#! /usr/bin/env python3

# Copyright (c) 2024 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
#
# SPDX-License-Identifier: GPL-2.0-only

import argparse
import glob
import itertools
import os
import re
import shutil
import sys

# Check for dependencies that are not in the standard Python libraries
try:
    import git.exc
    from git import Repo
except ImportError:
    raise Exception("Missing pip package 'GitPython'.")

if shutil.which("git") is None:
    raise Exception("Missing program 'git'.")

# Define global variables
CTTC_LENA_NR_REPOSITORY = "https://gitlab.com/cttc-lena/nr"
COMMIT_STYLE_REGEX = r"^([a-z0-9\-\*]+(?:, [a-z0-9\-\*]+)*):((?: \(\w+ #\d+\))?) ([A-Z][^\n]*$)"


# Get all file names without extension
def get_files_dictionary(list_of_files):
    files_ext = list_of_files
    files_wext = list(map(lambda x: os.path.splitext(os.path.basename(x))[0], files_ext))
    files_wext = list(map(lambda x: x[1:] if x[0] == "." else x, files_wext))
    files_wext = list(map(lambda x: x.lower(), files_wext))
    return dict(zip(files_wext, files_ext))


CURRENT_FILE_DIRECTORY = os.path.dirname(os.path.abspath(__file__))
NR_TOP_LEVEL_DIRECTORY = os.path.abspath(f"{CURRENT_FILE_DIRECTORY}/../../")
FILES = get_files_dictionary(
    glob.glob("./**/*", root_dir=NR_TOP_LEVEL_DIRECTORY, recursive=True)
    + glob.glob("./**/.*", root_dir=NR_TOP_LEVEL_DIRECTORY, recursive=True)
)


# Function definitions
def ensure_upstream_remote(repo: Repo):
    remotes = list(repo.remotes)
    # First search if cttc-lena/nr is already a remote
    for remote in remotes:
        remote_urls = list(remote.urls)
        if CTTC_LENA_NR_REPOSITORY in remote_urls:
            return remote
    # If it isn't, add the new remote
    used_remote_names = list(map(lambda x: x.name, remotes))
    possible_remote_names_to_use = ["origin", "upstream", "cttc-nr"]
    remote_name_to_use = list(
        filter(lambda x: x not in used_remote_names, possible_remote_names_to_use)
    )[0]
    return repo.create_remote(remote_name_to_use, url=CTTC_LENA_NR_REPOSITORY)


def ensure_master_is_up_to_date(repo, remote, update_master_to_upstream=False):
    # Fetch the latest commits from upstream
    remote.fetch()

    # Create master branch from upstream
    master_branch = list(filter(lambda x: "master" in x.name, list(repo.heads)))
    if update_master_to_upstream:
        if master_branch:
            repo.delete_head(master_branch[0].name, force=True)
        repo.create_head("master", remote.refs.master)
        try:
            remote.pull("master", ff_only=True)
        except git.exc.GitCommandError as e:
            if "Not possible to fast-forward" in e.stderr:
                print(f"Rebase HEAD on top of 'master'")
                exit(-1)
            raise e
    else:
        if not master_branch:
            repo.create_head("master", remote.refs.master)


def get_new_commits(repo):
    head_commits = list(repo.iter_commits(rev="HEAD"))
    upstream_commits = list(repo.iter_commits(rev="master"))
    diff_commits = list(filter(lambda x: x not in upstream_commits, head_commits))
    return diff_commits


class BaseCheck:
    def __init__(self, args=None):
        self.omit_commit_number = args.omit_commit_number

    # To be redefined by the class children
    def check(self, commit_info):
        return False

    def description(self):
        return None

    def diagnostic_message(self, commit_info):
        return ""

    # Virtual methods
    def execute_check(self, commit_info):
        return self.check(commit_info)

    def execute_description(self):
        return self.description()

    def execute_diagnostic(self, commit_info):
        commit_number = f"'{commit_info.hex}' "
        if self.omit_commit_number:
            commit_number = ""
        msg = f"  Commit {commit_number}with Message: '{commit_info.header}'\n"
        return msg + self.diagnostic_message(commit_info)


class LengthCheck(BaseCheck):
    MAX = 100

    def check(self, commit_info):
        return len(commit_info.header) <= LengthCheck.MAX

    def description(self):
        return f"Checked rule: commits should have no more than 100 characters."

    def diagnostic_message(self, commit_info):
        commit_number = f"{commit_info.hex} "
        if self.omit_commit_number:
            commit_number = ""
        diff = len(commit_info.header) - LengthCheck.MAX
        msg = f"  \tError: commit {commit_number}has {LengthCheck.MAX+diff} characters\n"
        msg += f"  \t\t{commit_info.header}\n"
        msg += f"  \t\t{' '*(len(commit_info.header)-diff)}{'^'*diff}"
        return msg


class MatchesFormatCheck(BaseCheck):
    def check(self, commit_info):
        return commit_info.matches is not None

    def description(self):
        msg = (
            "Checked rule: commit should match the format style 'nr: (fixes #1) Correct commit'.\n"
        )
        msg += "                                               prefix-^ optional-^ description-^"
        return msg

    def diagnostic_message(self, commit_info):
        header_with_newline = commit_info.header + "\n"
        errors = []
        # A big if
        once = 1
        while once:
            once -= 1
            # Check if there is a colon separating the prefix from the message
            try:
                prefix_separator_pos = header_with_newline.index(":")
            except ValueError:
                # There is not much more we can check at this point
                errors.append("missing prefix separator ':'")
                break

            # If there is a prefix separator, get the prefix
            prefix = header_with_newline[:prefix_separator_pos]
            message = header_with_newline[prefix_separator_pos + 1 :]

            # Check for upper case letters
            if prefix != prefix.lower():
                errors.append("prefix contains upper case letters")

            # Check for missing whitespaces after commas
            if prefix.count(",") != prefix.count(", "):
                errors.append("prefix has missing whitespaces after commas")

            # Check if there are dots
            if prefix.count("."):
                errors.append(
                    "prefix should not contain file extensions or the hidden file initial dot '.'"
                )

            # Check if there is an optional fixes field
            if message.count("fixes"):
                # Separate optional field from the rest of the message
                fixes_pos = message.index("fixes")
                # Check if it is missing the left parenthesis
                if message[:fixes_pos].count("(") == 0:
                    errors.append("missing optional field left parenthesis")

                # Check if it is missing the right parenthesis
                if message[fixes_pos:].count("(") == 0:
                    errors.append("missing optional field right parenthesis")

                # Check if we have a space and a hashtag after fixes
                fixes_end_pos = fixes_pos + len("fixes")
                if message[fixes_end_pos : fixes_end_pos + 2] != " #":
                    errors.append("missing space and/or hashtag after 'fixes'")
            else:
                # Check if 'fixes' is spelled as 'fixes'
                if message.count("fix #"):
                    errors.append("optional field should be '(fixes #issue)'")

            # Check if parenthesis are matching
            if message.count("(") != message.count(")"):
                errors.append("mismatching parenthesis")

            # If there is a closing parenthesis, get the commit description after it
            if message.count(")"):
                message = message[message.index(")") + 1 :]

            # Remove whitespaces and newlines
            message = message.strip()
            if message[0].upper() != message[0]:
                errors.append("description should start with upper case letters")
            pass

        msg = f"  \tErrors:"
        msg += "".join(map(lambda x: f"\n\t\t{x}", errors))
        return msg


class CheckPrefix(BaseCheck):
    def check(self, commit_info):
        # Can't check prefix if it doesn't match the expected format
        if commit_info.matches is None:
            return False

        # Extract changed files in the commit
        changed_files = get_files_dictionary(list(commit_info.commit.stats.files.keys()))

        # Extract the components from the first regex group (list of files before ':')
        components = commit_info.matches.group(1).split(", ")

        if len(changed_files) < 4:
            # Test the case with less than 4 modified files
            for component in components:
                if component.endswith("*"):
                    matching_files = list(filter(lambda x: component[:-1] in x, FILES))
                    if len(matching_files) == 0:
                        return False
                    matching_changed_files = list(
                        filter(lambda x: x in changed_files, matching_files)
                    )
                    if len(matching_changed_files) == 0:
                        return False
                else:
                    if component not in FILES:
                        return False
                    if component not in changed_files:
                        return False
            return True
        else:
            # Test the case with more than 3 modified files
            # There should be a single component
            if len(components) != 1:
                return False

            # Get the common path between the changed files
            parent_dir = os.path.commonpath(
                list(map(lambda x: os.path.join("nr", x), changed_files.values()))
            )
            parent_dir = os.path.basename(parent_dir)

            # Check if that path matches the component prefix
            return components[0] == parent_dir

    def description(self):
        msg = (
            "  Checked rules:\n"
            "   1.Commits with up to 3 changed files should use them as a prefix.\n"
            "   2.Commits with more than 3 changed files should use their closest common parent directory as a prefix."
        )
        return msg

    def diagnostic_message(self, commit_info):
        if commit_info.matches is None:
            return f"  \tError: incorrect format style"

        # Extract components from prefix
        components = commit_info.matches.group(1).split(", ")

        # Extract changed files in the commit
        changed_files = get_files_dictionary(list(commit_info.commit.stats.files.keys()))

        if len(changed_files) < 4:
            # Check each component
            errors = []

            # If the names of the files are bigger than 40 characters,
            # look for similarities, so that we can save up on the prefix length
            if len(", ".join(changed_files.keys())) > 40:
                # Find the largest shared prefix
                all_combinations = itertools.combinations(changed_files.keys(), len(changed_files))
                max_prefix = ""
                for combination in all_combinations:
                    prefix = os.path.commonprefix(combination)
                    if len(prefix) > len(max_prefix):
                        max_prefix = prefix

                # Regenerate prefix with shared prefix
                changed_files_not_sharing_prefix = list(
                    filter(lambda x: max_prefix not in x, changed_files.keys())
                )
                shorter_prefix = ", ".join(changed_files_not_sharing_prefix + [f"{max_prefix}-*"])
                if shorter_prefix not in commit_info.header:
                    errors.append(f'Prefix is too large: use "{shorter_prefix}"')

            for component in components:
                if component.endswith("*"):
                    matching_files = list(filter(lambda x: component[:-1] in x, FILES))
                    if len(matching_files) == 0:
                        errors.append(f"{component} is not an existing file")
                        continue
                    matching_changed_files = list(
                        filter(lambda x: x in changed_files, matching_files)
                    )
                    if len(matching_changed_files) == 0:
                        errors.append(f"{matching_files} have not been changed")
                else:
                    if component not in FILES:
                        errors.append(f"{component} is not an existing file")
                        continue
                    if component not in changed_files:
                        errors.append(f"{FILES[component]} has not been changed")
            return f"  \tErrors: {', '.join(errors)}"
        else:
            # Get the common path between the changed files
            parent_dir = os.path.commonpath(
                list(map(lambda x: os.path.join("nr", x), changed_files.values()))
            )
            parent_dir = os.path.basename(parent_dir)

            if len(components) != 1 or components[0] != parent_dir:
                return f"  \tError: prefix should be '{parent_dir}'"
        return None


def retrieve_commits_info():
    try:
        repo = Repo(NR_TOP_LEVEL_DIRECTORY)
    except git.exc.InvalidGitRepositoryError:
        raise "The current directory does not contain a .git directory"

    # Retrieve the upstream remote
    upstream_remote = ensure_upstream_remote(repo)

    # Fetch the latest master branch
    ensure_master_is_up_to_date(repo, upstream_remote)

    class CommitInfo:
        def __init__(self, commit):
            self.commit = commit
            self.hex = commit.hexsha[:8]
            self.header = commit.message.split("\n")[0]
            self.matches = re.match(COMMIT_STYLE_REGEX, self.header + "\n")

    # Get new commits when compared to upstream and extract info
    return list(map(CommitInfo, get_new_commits(repo)))


def main(argv):
    parser = argparse.ArgumentParser()
    parser.add_argument("--omit-commit-number", action="store_true")
    args = parser.parse_known_args(argv)[0]
    commits_info = retrieve_commits_info()

    checks = [
        LengthCheck(args),
        MatchesFormatCheck(args),
        CheckPrefix(args),
    ]

    passed = 0
    failed = 0
    failed_tests = {}
    for check in checks:
        failed_tests[check] = []
        for commit_info in commits_info:
            result = check.execute_check(commit_info)
            # Skipped for not being relevant
            if result is None:
                continue
            # Successfully passed
            if result is True:
                passed += 1
                continue

            # Failed checks
            failed += 1
            failed_tests[check].append(commit_info)

    print(f"Passed: {passed}, Failed: {failed}")
    for check, failed_commits in failed_tests.items():
        # Skip checks with no failures
        if not failed_commits:
            continue
        print(f"Commits that failed check '{check.__class__.__name__}'")
        print(check.execute_description())
        for commit in failed_commits:
            print(check.execute_diagnostic(commit))

    exit(failed)


# To add new test cases, apply patch
#   git am ./tools/check-commit-message/test/test-cases.patch
# Create new commits, then update the patch file
#   git format-patch StartingHex^..FinalHex --stdout > ./tools/check-commit-message/test/test-cases.patch
# Then run
#   ./tools/check-commit-message/check-commit-message.py --omit-commit-number > ./tools/check-commit-message/test/reference-output.txt
# Commit the patch and reference output changes, then drop the test case commits (StartingHex^..FinalHex)

# To simply run the tests, apply the patch
#   git am ./tools/check-commit-message/test/test-cases.patch
# Then run
#   ./tools/check-commit-message/check-commit-message.py --omit-commit-number > out.txt
# And compare the outputs
#   diff ./tools/check-commit-message/test/reference-output.txt out.txt

if __name__ == "__main__":
    main(sys.argv[1:])
