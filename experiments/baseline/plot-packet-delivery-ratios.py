import pandas as pd
from matplotlib import pyplot as plt
from glob import glob
import re

packet_delivery_ratio_flowmon_csvs = glob("packet-delivery-ratio-flowmon-*.csv")

for flowmon_csv in packet_delivery_ratio_flowmon_csvs:
    scenario_id = re.search(r'\d+', flowmon_csv).group()
    flowmon_frame = pd.read_csv(flowmon_csv)
    flowmon_frame.plot(x="TimeSeconds", y=" PacketDeliveryRatio",
                       title=f"Packet Delivery Ratio - Flow Monitor-{scenario_id}")
    plt.savefig(f"packet-delivery-ratio_flowmon-{scenario_id}.png")


packet_delivery_ratio_olsr_csvs = glob("packet-delivery-ratio_olsr-traces-*.csv")
for olsr_csv in packet_delivery_ratio_olsr_csvs:
    scenario_id = re.search(r'\d+', olsr_csv).group()
    olsr_frame = pd.read_csv(olsr_csv)
    olsr_frame.plot(x="TimeSeconds", y=" PacketDeliveryRatio",
                       title=f"Packet Delivery Ratio - OLSR Traces- Scenario: {scenario_id}")

    plt.savefig(f"packet-delivery-ratio_olsr-traces-{scenario_id}.png")
