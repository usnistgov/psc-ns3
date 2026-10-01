// Copyright (c) 2020 Lawrence Livermore National Laboratory
//
// SPDX-License-Identifier: GPL-2.0-only

#ifndef FILE_SCENARIO_HELPER_H
#define FILE_SCENARIO_HELPER_H

#include "node-distribution-scenario-interface.h"

#include <ns3/ptr.h>
#include <ns3/vector.h>

namespace ns3
{

// Forward declaration
class ListPositionAllocator;

/**
 * @brief The FileScenarioHelper class
 *
 * This scenario helper reads site locations from a CSV file.
 * First Add() the positions, then CreateScenario().
 * Most Get() functions won't be valid before those two steps.
 *
 * \todo Documentation, tests
 */
class FileScenarioHelper : public NodeDistributionScenarioInterface
{
  public:
    /**
     * \brief ~FileScenarioHelper
     */
    ~FileScenarioHelper() override;

    /**
     * \brief Add the positions listed in a file.
     * The file should be a simple text file, with one position per line,
     * either X and Y, or X, Y and Z, in meters.  The delimiter can
     * be any character, such as ',' or '\t'; the default is a comma ','.
     *
     * The file is read using CsvReader, which explains how comments
     * and whitespace are handled.
     *
     * Multiple calls to Add() will append the positions from each
     * successive file to the list of sites.
     *
     * \param [in] filePath The path to the input file.
     * \param [in] delimiter The delimiter character; see CsvReader.
     */
    void Add(const std::string filePath, char delimiter = ',');

    /**
     * \brief Get the site position corresponding to a given cell.
     * \param cellId the cell ID of the antenna
     */
    Vector GetSitePosition(std::size_t cellId) const;

    // inherited
    void CreateScenario() override;
    using NodeDistributionScenarioInterface::GetAntennaPosition;

  private:
    /**
     * \brief Check that we have created the scenario before
     * returning values.
     * Asserts if CheckScenario hasn't been run.
     * \param where The function doing the check.
     */
    void CheckScenario(const char* where) const;

    /** Have we created the scenario yet? */
    bool m_scenarioCreated{false};

    /**
     * The position allocator holding the actual site positions
     * read from the file.
     */
    Ptr<ListPositionAllocator> m_bsPositioner;
};

} // namespace ns3

#endif /* FILE_SCENARIO_HELPER_H */
