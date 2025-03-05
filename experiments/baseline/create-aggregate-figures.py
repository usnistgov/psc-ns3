import json
import pandas as pd
from matplotlib import pyplot as plt
from matplotlib import ticker
from glob import glob

scenario_infos = []
scenario_info_files = glob('scenario-info-*.json')
for scenario_info_file in scenario_info_files:
    with open(scenario_info_file) as f:
        scenario_infos.append(json.load(f))

scenario_infos.sort(key=lambda item: item['scenarioId'])


def get_suffix(info):
    return f"{info['protocol']}-{info['scenarioId']}"

# Figure A:
# Average overhead vs Speed
overhead_frames = []
for scenario_info in scenario_infos:
    overhead_frames.append(pd.read_csv(f'olsr-overhead-{get_suffix(scenario_info)}.csv'))

index = 0
aggregate_overhead_frame = pd.DataFrame(columns=['Speed', 'Protocol', 'OverheadBytesAverage'])
for overhead_frame in overhead_frames:
    scenario_info = scenario_infos[index]

    total_time = scenario_info['simulationTimeSeconds'] - scenario_info['startTimeSeconds']
    total_overhead = overhead_frame.loc[len(overhead_frame) - 1]['TxBytesTotal']
    average_overhead = total_overhead / total_time

    line_frame = pd.DataFrame(data={'Speed': [scenario_info['speed']],
                                    'Protocol': [scenario_info['protocol']],
                                    'OverheadBytesAverage': [average_overhead]})

    # Pandas does not like appending to an empty dataframe...
    if len(aggregate_overhead_frame) == 0:
        aggregate_overhead_frame = line_frame
    else:
        aggregate_overhead_frame = pd.concat([aggregate_overhead_frame, line_frame])

    index += 1

aggregate_overhead_frame = aggregate_overhead_frame.pivot(index='Speed', columns='Protocol', values='OverheadBytesAverage')

plt.plot(aggregate_overhead_frame['OLSR'], marker='o')
plt.plot(aggregate_overhead_frame['OLSRv2'], marker='*')
plt.xlabel('Speed (m/s)')
plt.ylabel('Average Overhead (bytes/s)')
plt.ylim(0, None)
plt.title("Speed vs Average Overhead")
plt.legend(['OLSR', 'OLSR/NHDP'])

plt.savefig('Average-Overhead-vs-Speed.png')
plt.close()

# Figure B:
# Application Packet Delivery Ratio vs Speed
packet_delivery_ratio_frames = []
for scenario_info in scenario_infos:
    packet_delivery_ratio_frames.append(pd.read_csv(f'app-tx-rx-{get_suffix(scenario_info)}.csv'))

aggregate_packet_delivery_ratio = pd.DataFrame()
index = 0
for packet_delivery_ratio_frame in packet_delivery_ratio_frames:
    scenario_info = scenario_infos[index]

    total_time = scenario_info['simulationTimeSeconds'] - scenario_info['startTimeSeconds']

    frame_max_index = len(packet_delivery_ratio_frame) - 1
    totalTx = packet_delivery_ratio_frame.loc[frame_max_index]['TxPacketsTotal']
    totalRx = packet_delivery_ratio_frame.loc[frame_max_index]['RxPacketsTotal']
    total_packet_delivery_ratio = totalRx / totalTx

    append_data = pd.DataFrame(data={
        'Speed': [scenario_info['speed']],
        'Protocol': [scenario_info['protocol']],
        'PacketDeliveryRatio': [total_packet_delivery_ratio]
    })

    # Pandas does not like appending to an empty dataframe...
    if len(aggregate_packet_delivery_ratio) > 0:
        aggregate_packet_delivery_ratio = pd.concat([aggregate_packet_delivery_ratio, append_data])
    else:
        aggregate_packet_delivery_ratio = append_data

    index += 1

aggregate_packet_delivery_ratio = aggregate_packet_delivery_ratio.pivot(index='Speed', columns='Protocol', values='PacketDeliveryRatio')

_, ax = plt.subplots()
ax.yaxis.set_major_formatter(ticker.PercentFormatter(xmax=1.0, decimals=1))

plt.plot(aggregate_packet_delivery_ratio['OLSR'], marker='o')
plt.plot(aggregate_packet_delivery_ratio['OLSRv2'], marker='*')
plt.xlabel('Speed (m/s)')
plt.ylabel('Packet Delivery Ratio')
plt.title("Speed vs Packet Delivery Ratio")
plt.legend(['OLSR', 'OLSR/NHDP'])

plt.savefig('Speed-vs-Packet-Delivery-Ratio.png')
plt.close()


# Average Hop Count vs Speed
hop_count_frames = []
for scenario_info in scenario_infos:
    hop_count_frames.append(pd.read_csv(f'hop-count-{get_suffix(scenario_info)}.csv'))


aggregate_hop_count = pd.DataFrame()
index = 0
for hop_count_frame in hop_count_frames:
    scenario_info = scenario_infos[index]

    total_time = scenario_info['simulationTimeSeconds'] - scenario_info['startTimeSeconds']

    frame_max_index = len(hop_count_frame) - 1
    total_hops = hop_count_frame.loc[frame_max_index]['HopsTotal']
    average_hops = total_hops/total_time

    append_data = pd.DataFrame(data={
        'Speed': [scenario_info['speed']],
        'Protocol': [scenario_info['protocol']],
        'AverageHops': [average_hops]
    })
    if len(aggregate_hop_count) > 0:
        aggregate_hop_count = pd.concat([aggregate_hop_count, append_data])
    else:
        aggregate_hop_count = append_data

    index += 1

aggregate_hop_count = aggregate_hop_count.pivot(index='Speed', columns='Protocol', values='AverageHops')

plt.plot(aggregate_hop_count['OLSR'], marker='o')
plt.plot(aggregate_hop_count['OLSRv2'], marker='*')
plt.xlabel('Speed (m/s)')
plt.ylabel('Average Hops')
plt.title("Speed vs Average Hops")
plt.legend(['OLSR', 'OLSR/NHDP'])

plt.savefig('Speed-vs-Average-Hops.png')
plt.close()
