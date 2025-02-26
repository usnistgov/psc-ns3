import json
import pandas as pd
from matplotlib import pyplot as plt
from glob import glob

scenario_infos = []
scenario_info_files = glob("scenario-info-*.json")
for scenario_info_file in scenario_info_files:
    with open(scenario_info_file) as f:
        scenario_infos.append(json.load(f))

scenario_infos.sort(key=lambda item: item["scenarioId"])

# Figure A:
# Average overhead vs Speed
overhead_frames = []
for scenario_info in scenario_infos:
    overhead_frames.append(pd.read_csv(f"olsr-overhead-{scenario_info["scenarioId"]}.csv"))

index = 0
aggregate_overhead_frame = pd.DataFrame(columns=['Speed', 'OverheadBytesAverage'])
for overhead_frame in overhead_frames:
    scenario_info = scenario_infos[index]

    total_time = scenario_info["simulationTimeSeconds"] - scenario_info["startTimeSeconds"]
    total_overhead = overhead_frame.loc[len(overhead_frame) - 1]["TxBytesTotal"]
    average_overhead = total_overhead / total_time

    line_frame = pd.DataFrame(data={'Speed': [scenario_info['speed']], 'OverheadBytesAverage': [average_overhead]})

    # Pandas does not like appending to an empty dataframe...
    if len(aggregate_overhead_frame) == 0:
        aggregate_overhead_frame = line_frame
    else:
        aggregate_overhead_frame = pd.concat([aggregate_overhead_frame, line_frame])

    index += 1

aggregate_overhead_frame.plot(x='Speed', xlabel='Speed (m/s)', y='OverheadBytesAverage',
                              ylabel='Average Overhead (bytes/s)', title='Average Overhead vs Speed', legend=False)
plt.savefig("Average Overhead vs Speed.png")
plt.close()

# Figure B:
# Application Packet Delivery Ratio vs Speed
packet_delivery_ratio_frames = []
for scenario_info in scenario_infos:
    packet_delivery_ratio_frames.append(pd.read_csv(f"app-tx-rx-{scenario_info['scenarioId']}.csv"))

aggregate_packet_delivery_ratio = pd.DataFrame()
index = 0
for packet_delivery_ratio_frame in packet_delivery_ratio_frames:
    scenario_info = scenario_infos[index]

    total_time = scenario_info["simulationTimeSeconds"] - scenario_info["startTimeSeconds"]

    frame_max_index = len(packet_delivery_ratio_frame) - 1
    totalTx = packet_delivery_ratio_frame.loc[frame_max_index]['TxPacketsTotal']
    totalRx = packet_delivery_ratio_frame.loc[frame_max_index]['RxPacketsTotal']
    total_packet_delivery_ratio = totalRx / totalTx

    append_data = pd.DataFrame(data={
        'Speed': [scenario_info['speed']],
        'PacketDeliveryRatio': [total_packet_delivery_ratio]
    })

    # Pandas does not like appending to an empty dataframe...
    if len(aggregate_packet_delivery_ratio) > 0:
        aggregate_packet_delivery_ratio = pd.concat([aggregate_packet_delivery_ratio, append_data])
    else:
        aggregate_packet_delivery_ratio = append_data

    index += 1
aggregate_packet_delivery_ratio.plot(x='Speed', xlabel='Speed (m/s)', y='PacketDeliveryRatio',
                                     ylabel='Packet Delivery Ratio', title='Speed vs Packet Delivery Ratio', style='-o',
                                     legend=False)
plt.savefig("Speed vs Packet Delivery Ratio.png")
plt.close()
