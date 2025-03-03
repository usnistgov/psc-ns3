import pandas as pd
from matplotlib import pyplot as plt
import argparse

parser = argparse.ArgumentParser('OLSR Study Plotting Helper')
parser.add_argument('scenario_id', help='the sequential ID of the scenario, used to identify output files', type=int)
args = parser.parse_args()

scenario_id = args.scenario_id


# flowmonitor-totals-*.csv
flowmon_frame = pd.read_csv(f'flowmonitor-totals-{scenario_id}.csv')
flowmon_frame.plot(x='TimeSeconds', y='PacketDeliveryRatio',
                   title=f'Packet Delivery Ratio - Flow Monitor-{scenario_id}')
plt.savefig(f'packet-delivery-ratio_flowmon-{scenario_id}.png')
plt.close()

# packet-delivery-ratio_olsr-traces-*.csv
olsr_frame = pd.read_csv(f'packet-delivery-ratio_olsr-traces-{scenario_id}.csv')
olsr_frame.plot(x='TimeSeconds', y='PacketDeliveryRatio',
                title=f'Packet Delivery Ratio - OLSR Traces- Scenario: {scenario_id}')

plt.savefig(f'packet-delivery-ratio_olsr-traces-{scenario_id}.png')
plt.close()


# flowmon-per-flow-*.csv
def plot_flow(flow_id: int):
    one_flow = flowmon_per_flow.loc[flowmon_per_flow['FlowId'] == flow_id]
    source_ip = one_flow['SourceIp'].unique()[0]
    destination_ip = one_flow['DestinationIp'].unique()[0]
    one_flow.plot(x='TimeSeconds', y='PacketDeliveryRatio',
                  title=f'Flow {flow_id} - Src: {source_ip} - Dst: {destination_ip}')

    plt.savefig(f'flow-scenario-{scenario_id}-id-{flow_id}.png')
    plt.close()


flowmon_per_flow = pd.read_csv(f'flowmon-per-flow-{scenario_id}.csv')
flows = flowmon_per_flow['FlowId'].unique()

for flow in flows:
    plot_flow(flow)


# routing-table-changes-*.csv
routing_table_changes = pd.read_csv(f'routing-table-changes-{scenario_id}.csv')
routing_table_changes.plot(x='TimeSeconds', xlabel='Seconds', y='PeriodRoutingTableChanges', ylabel='Changes',
                           title='Routing Table Changes Per Second', legend=False)

plt.savefig(f'routing-table-changes-scenario-{scenario_id}.png')
plt.close()


# olsr-overhead-*.csv
olsr_overhead = pd.read_csv(f'olsr-overhead-{scenario_id}.csv')
olsr_overhead.plot(x='TimeSeconds', xlabel='Seconds', y='TxBytesPeriod', ylabel='Bytes per Second',
                   title=f'OLSR Overhead - Scenario {scenario_id}', legend=False)
plt.savefig(f'olsr-overhead-{scenario_id}.png')
plt.close()
