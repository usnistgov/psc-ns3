import pandas as pd
from matplotlib import pyplot as plt
from glob import glob
import re

flowmon_totals = glob("flowmonitor-totals-*.csv")

for flowmon_total_csv in flowmon_totals:
    scenario_id = re.search(r'\d+', flowmon_total_csv).group()
    flowmon_frame = pd.read_csv(flowmon_total_csv)
    flowmon_frame.plot(x="TimeSeconds", y="PacketDeliveryRatio",
                       title=f"Packet Delivery Ratio - Flow Monitor-{scenario_id}")
    plt.savefig(f"packet-delivery-ratio_flowmon-{scenario_id}.png")
    plt.close()


packet_delivery_ratio_olsr_csvs = glob("packet-delivery-ratio_olsr-traces-*.csv")
for olsr_csv in packet_delivery_ratio_olsr_csvs:
    scenario_id = re.search(r'\d+', olsr_csv).group()
    olsr_frame = pd.read_csv(olsr_csv)
    olsr_frame.plot(x="TimeSeconds", y="PacketDeliveryRatio",
                    title=f"Packet Delivery Ratio - OLSR Traces- Scenario: {scenario_id}")

    plt.savefig(f"packet-delivery-ratio_olsr-traces-{scenario_id}.png")
    plt.close()


flowmon_per_flow_csvs = glob("flowmon-per-flow-*.csv")

for flowmon_per_flow_csv in flowmon_per_flow_csvs:
    scenario_id = re.search(r'\d+', flowmon_per_flow_csv).group()

    flowmon_per_flow = pd.read_csv(flowmon_per_flow_csv)
    flows = flowmon_per_flow["FlowId"].unique()
    for flow_id in flows:
        one_flow = flowmon_per_flow.loc[flowmon_per_flow["FlowId"] == flow_id]
        source_ip = one_flow["SourceIp"].unique()[0]
        destination_ip = one_flow["DestinationIp"].unique()[0]
        one_flow.plot(x="TimeSeconds", y="PacketDeliveryRatio", title=f"Flow {flow_id} - Src: {source_ip} - Dst: {destination_ip}")

        plt.savefig(f"flow-scenario-{scenario_id}-id-{flow_id}.png")
        plt.close()



routing_table_changes_csvs = glob("routing-table-changes-*.csv")
for routing_table_changes_csv in routing_table_changes_csvs:
    scenario_id = re.search(r'\d+', routing_table_changes_csv).group()
    routing_table_changes = pd.read_csv(routing_table_changes_csv)
    routing_table_changes.plot(x="TimeSeconds", xlabel="Seconds", y="PeriodRoutingTableChanges", ylabel="Changes",
                               title="Routing Table Changes Per Second", legend=False)

    plt.savefig(f"routing-table-changes-scenario-{scenario_id}.png")
    plt.close()


