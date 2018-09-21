/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * NIST-developed software is provided by NIST as a public
 * service. You may use, copy and distribute copies of the software in
 * any medium, provided that you keep intact this entire notice. You
 * may improve, modify and create derivative works of the software or
 * any portion of the software, and you may copy and distribute such
 * modifications or works. Modified works should carry a notice
 * stating that you changed the software and should note the date and
 * nature of any such change. Please explicitly acknowledge the
 * National Institute of Standards and Technology as the source of the
 * software.
 *
 * NIST-developed software is expressly provided "AS IS." NIST MAKES
 * NO WARRANTY OF ANY KIND, EXPRESS, IMPLIED, IN FACT OR ARISING BY
 * OPERATION OF LAW, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTY OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE,
 * NON-INFRINGEMENT AND DATA ACCURACY. NIST NEITHER REPRESENTS NOR
 * WARRANTS THAT THE OPERATION OF THE SOFTWARE WILL BE UNINTERRUPTED
 * OR ERROR-FREE, OR THAT ANY DEFECTS WILL BE CORRECTED. NIST DOES NOT
 * WARRANT OR MAKE ANY REPRESENTATIONS REGARDING THE USE OF THE
 * SOFTWARE OR THE RESULTS THEREOF, INCLUDING BUT NOT LIMITED TO THE
 * CORRECTNESS, ACCURACY, RELIABILITY, OR USEFULNESS OF THE SOFTWARE.
 *
 * You are solely responsible for determining the appropriateness of
 * using and distributing the software and you assume all risks
 * associated with its use, including but not limited to the risks and
 * costs of program errors, compliance with applicable laws, damage to
 * or loss of data, programs or equipment, and the unavailability or
 * interruption of operation. This software is not intended to be used
 * in any situation where a failure could cause risk of injury or
 * damage to property. The software developed by NIST employees is not
 * subject to copyright protection within the United States.
 */

#include <ns3/lte-3gpp-hex-grid-enb-topology-helper.h>
#include <ns3/double.h>
#include <ns3/log.h>
#include <ns3/abort.h>
#include <ns3/pointer.h>
#include <ns3/epc-helper.h>
#include <ns3/angles.h>
#include <ns3/random-variable-stream.h>
#include <ns3/lte-enb-net-device.h>
#include <ns3/lte-enb-phy.h>
#include <ns3/lte-ue-net-device.h>
#include <ns3/lte-ue-phy.h>
#include <ns3/lte-spectrum-phy.h>
#include <ns3/building.h>
#include <iostream>
#include <iomanip>


namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("Lte3gppHexGridEnbTopologyHelper");

NS_OBJECT_ENSURE_REGISTERED (Lte3gppHexGridEnbTopologyHelper);

TypeId Lte3gppHexGridEnbTopologyHelper::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::Lte3gppHexGridEnbTopologyHelper")
    .SetParent<Object> ()
    .AddConstructor<Lte3gppHexGridEnbTopologyHelper> ()
    .AddAttribute ("InterSiteDistance",
                   "The distance [m] between nearby sites",
                   DoubleValue (500),
                   MakeDoubleAccessor (&Lte3gppHexGridEnbTopologyHelper::SetInterSiteDistance,
                                       &Lte3gppHexGridEnbTopologyHelper::GetInterSiteDistance),
                   MakeDoubleChecker<double> ())
    .AddAttribute ("SiteHeight",
                   "The height [m] of each site",
                   DoubleValue (30),
                   MakeDoubleAccessor (&Lte3gppHexGridEnbTopologyHelper::m_siteHeight),
                   MakeDoubleChecker<double> ())
    .AddAttribute ("UeHeight",
                   "The height [m] of each UE",
                   DoubleValue (1.5),
                   MakeDoubleAccessor (&Lte3gppHexGridEnbTopologyHelper::m_ueHeight),
                   MakeDoubleChecker<double> ())
    .AddAttribute ("NumberOfRings",
                   "The number of rings in the grid",
                   UintegerValue (1),
                   MakeUintegerAccessor (&Lte3gppHexGridEnbTopologyHelper::SetNumRings,
                                         &Lte3gppHexGridEnbTopologyHelper::GetNumRings),
                   MakeUintegerChecker<uint32_t> ())
    .AddAttribute ("CenterX", "The x coordinate of the center of the hex grid.",
                   DoubleValue (0.0),
                   MakeDoubleAccessor (&Lte3gppHexGridEnbTopologyHelper::m_xCenter),
                   MakeDoubleChecker<double> ())
    .AddAttribute ("CenterY", "The y coordinate of the center of the hex grid.",
                   DoubleValue (0.0),
                   MakeDoubleAccessor (&Lte3gppHexGridEnbTopologyHelper::m_yCenter),
                   MakeDoubleChecker<double> ())
    .AddAttribute ("MinDistance", "The minimum distance between eNodeB and UE.",
                   DoubleValue (1.0),
                   MakeDoubleAccessor (&Lte3gppHexGridEnbTopologyHelper::SetMinimumDistance),
                   MakeDoubleChecker<double> ())
    .AddAttribute ("MinEnodeb2HotspotDistance", "The minimum distance between eNodeB and hotspot.",
                   DoubleValue (75.0),
                   MakeDoubleAccessor (&Lte3gppHexGridEnbTopologyHelper::m_minEnodeb2HotspotDistance),
                   MakeDoubleChecker<double> ())
    .AddAttribute ("MinHotspotToHotspotDistance", "The minimum distance between hotspot centers.",
                   DoubleValue (80.0),
                   MakeDoubleAccessor (&Lte3gppHexGridEnbTopologyHelper::m_minHotspotToHotspotDistance),
                   MakeDoubleChecker<double> ())
    .AddAttribute ("HotspotRange", "The radius of the hotspot.",
                   DoubleValue (40.0),
                   MakeDoubleAccessor (&Lte3gppHexGridEnbTopologyHelper::m_hotspotRange),
                   MakeDoubleChecker<double> ())
    .AddAttribute ("MinEnodebToBuildingDistance", "The minimum distance between the center of the building and the eNodeB.",
                   DoubleValue (100.0),
                   MakeDoubleAccessor (&Lte3gppHexGridEnbTopologyHelper::m_minEnodebToBuildingDistance),
                   MakeDoubleChecker<double> ())
    .AddAttribute ("MinUeToUeDistanceInBuilding", "The minimum distance between 2 UEs inside a building.",
                   DoubleValue (3.0),
                   MakeDoubleAccessor (&Lte3gppHexGridEnbTopologyHelper::m_minUeToUeDistanceInBuilding),
                   MakeDoubleChecker<double> ())
  ;
  return tid;
}

Lte3gppHexGridEnbTopologyHelper::Lte3gppHexGridEnbTopologyHelper ()
{
  NS_LOG_FUNCTION (this);
  m_uniformRandomVariable = CreateObject<UniformRandomVariable> ();
}

Lte3gppHexGridEnbTopologyHelper::~Lte3gppHexGridEnbTopologyHelper (void)
{
  NS_LOG_FUNCTION (this);
}

void
Lte3gppHexGridEnbTopologyHelper::DoDispose ()
{
  NS_LOG_FUNCTION (this);
  Object::DoDispose ();
}


void
Lte3gppHexGridEnbTopologyHelper::SetLteHelper (Ptr<LteHelper> h)
{
  NS_LOG_FUNCTION (this << h);
  m_lteHelper = h;
}

void
Lte3gppHexGridEnbTopologyHelper::SetNumRings (uint32_t ring)
{
  NS_ABORT_MSG_IF (ring < 0, "Number of rings should be greater than zero");
  m_numRings = ring;
}

void
Lte3gppHexGridEnbTopologyHelper::SetCenter (double x, double y)
{
  m_xCenter = x;
  m_yCenter = y;
}

void
Lte3gppHexGridEnbTopologyHelper::SetInterSiteDistance (double d)
{
  NS_ABORT_MSG_IF (d < 0, "Inter site distance should be greater than zero");
  m_d = d;
}

void
Lte3gppHexGridEnbTopologyHelper::SetSiteHeight (double height)
{
  NS_ABORT_MSG_IF (height < 0, "eNB height should be greater than zero");
  m_siteHeight = height;
}

void
Lte3gppHexGridEnbTopologyHelper::SetUeHeight (double height)
{
  NS_ABORT_MSG_IF (height < 0, "UE height should be greater than zero");
  m_ueHeight = height;
}

void
Lte3gppHexGridEnbTopologyHelper::SetMinimumDistance (double m)
{
  NS_ABORT_MSG_IF (m > m_d / 2, "UE minimum distance from eNB must be less than ISD/2 = " << m_d / 2);
  m_minDistance = m;
}


NetDeviceContainer
Lte3gppHexGridEnbTopologyHelper::SetPositionAndInstallEnbDevice (NodeContainer c)
{
  NS_LOG_FUNCTION (this);
  NS_ABORT_MSG_IF (c.GetN () != GetNumNodes (), "To create " << m_numRings << " rings, given number of nodes should be equal to " << GetNumNodes ());
  NetDeviceContainer enbDevs;

  //Get node position and antenna orientation
  std::vector<Vector> eNodeBpos = GetNodePositions ();

  for (uint32_t n = 0; n < c.GetN (); )
    {
      for (uint32_t m = 0; m < 3; ++m,++n)
        {
          //position uses z for the antenna direction
          Vector pos = eNodeBpos.at (n);

          uint32_t currentSector = n % 3;

          //Compute antenna orientation and position
          m_lteHelper->SetFfrAlgorithmAttribute ("FrCellTypeId", UintegerValue (currentSector + 1));

          Ptr<Node> node = c.Get (n);
          Ptr<MobilityModel> mm = node->GetObject<MobilityModel> ();
          Vector pos2 (pos.x, pos.y, m_siteHeight);
          NS_LOG_LOGIC ("node " << n << " at " << pos << " antennaOrientation " << pos.z);
          mm->SetPosition (Vector (pos.x, pos.y, m_siteHeight));
          m_lteHelper->SetEnbAntennaModelAttribute ("Orientation", DoubleValue (pos.z));
          enbDevs.Add (m_lteHelper->InstallEnbDevice (node));
        }
    }

  return enbDevs;
}

double
Lte3gppHexGridEnbTopologyHelper::GetInterSiteDistance () const
{
  NS_LOG_FUNCTION (this);
  return m_d;
}


std::vector<Vector>
Lte3gppHexGridEnbTopologyHelper::GetNodePositions ()
{
  NS_LOG_FUNCTION (this);

  uint32_t currentRing = 0;
  uint32_t sitesInRing = 0;
  uint32_t currentSitesInRing = 0;
  double xSite = 0;
  double ySite = 0;
  std::vector<Vector> positions;

  for (uint32_t n = 0; n < GetNumNodes (); )
    {
      //Compute site location
      if (currentSitesInRing == sitesInRing)
        {
          //we completed the ring, move to the next one
          currentRing++;
          sitesInRing = currentRing == 1 ? 1 : (currentRing - 1) * 6;
          currentSitesInRing = 0;
          xSite = 0;
          ySite = (currentRing - 1) * m_d;
        }

      for (uint32_t m = 0; m < 3; ++m,++n)
        {
          uint32_t currentSector = n % 3;

          double antennaOrientation;

          //Compute antenna orientation and position (Ref:3GPP TR 36.814 Table A.2.1.1-2)
          antennaOrientation = 30 + 120 * currentSector;
          Vector pos (xSite, ySite, antennaOrientation);
          NS_LOG_LOGIC ("node " << n << " at " << pos << " antennaOrientation " << antennaOrientation);
          positions.push_back (pos);
        }

      if (currentRing > 1)
        {
          switch (currentSitesInRing / (currentRing - 1))
            {
            case 0:
              ySite -= m_d / 2;
              xSite -= m_d * std::cos (DegreesToRadians (30));
              break;
            case 1:
              ySite -= m_d;
              break;
            case 2:
              ySite -= m_d / 2;
              xSite += m_d * std::cos (DegreesToRadians (30));
              break;
            case 3:
              ySite += m_d / 2;
              xSite += m_d * std::cos (DegreesToRadians (30));
              break;
            case 4:
              ySite += m_d;
              break;
            case 5:
              ySite += m_d / 2;
              xSite -= m_d * std::cos (DegreesToRadians (30));
              break;
            default:
              //no default
              break;
            }
        }
      //increment counters of sites in ring
      currentSitesInRing++;
    }
  return positions;
}

NetDeviceContainer
Lte3gppHexGridEnbTopologyHelper::DropUEsUniformlyPerSector (NodeContainer ues)
{
  NS_LOG_FUNCTION (this);

  //make sure there will be equal number of UEs per cell
  NS_ABORT_MSG_IF (ues.GetN () % GetNumNodes () != 0, "There should be equal number of UEs per eNB. Total eNBs = " << GetNumNodes ());
  NS_ABORT_MSG_IF (m_minDistance > m_d / 2, "UE minimum distance from eNB must be less than ISD/2 = " << m_d / 2);

  //Storage for return value
  NetDeviceContainer ueDevs;

  uint32_t numUesPerSector = ues.GetN () / GetNumNodes ();
  uint32_t currentUe = 0;

  //Get node position and antenna orientation
  std::vector<Vector> eNodeBpos = GetNodePositions ();
  //Loop through each cell
  for (uint32_t n = 0; n < eNodeBpos.size (); n++)
    {
      //position uses z for the antenna direction
      Vector pos = eNodeBpos.at (n);
      double xSector, ySector;
      //Compute center of sector hexagon
      //This hexagon has center on cell flat edge
      xSector = pos.x + m_d / 2 * std::cos (DegreesToRadians (pos.z));
      ySector = pos.y + m_d / 2 * std::sin (DegreesToRadians (pos.z));
      NS_LOG_LOGIC ("sector_center " << n << " at " << xSector << ":" << ySector);

      //Drop users in the sector coverage
      for (uint32_t u = 0; u < numUesPerSector; u++)
        {
          double xPos, yPos;
          do
            {
              //pick random x and y
              xPos = pos.x + m_uniformRandomVariable->GetValue (-m_d, m_d);
              yPos = pos.y + m_uniformRandomVariable->GetValue (-m_d, m_d);
            }
          while (!(IsInsideHex (pos.x, pos.y, m_d / 2, xPos, yPos, true) && IsInsideHex (xSector, ySector, m_d / 2 * std::cos (DegreesToRadians (30)), xPos, yPos, false))
                 || GetDistance (pos.x, pos.y, xPos, yPos) < m_minDistance);
          // do while( Not(xPos, yPos are in the intersection of flat top hex and a pointy top hex whose center is on hex flat edge)
          // or distance < m_minDistance)

          //assign position to node
          Vector pos (xPos, yPos, m_ueHeight);
          Ptr<Node> node = ues.Get (currentUe);
          Ptr<MobilityModel> mm = node->GetObject<MobilityModel> ();
          NS_LOG_LOGIC ("ue " << u << " at " << pos);
          mm->SetPosition (Vector (xPos, yPos, m_ueHeight));
          ueDevs.Add (m_lteHelper->InstallUeDevice (node));

          //increment UE counter
          currentUe++;
        }

    }
  return ueDevs;
}

NetDeviceContainer
Lte3gppHexGridEnbTopologyHelper::DropUEsHotspot (NodeContainer ues)
{
  NS_LOG_FUNCTION (this);

  //make sure there will be equal number of UEs per cell
  NS_ABORT_MSG_IF (ues.GetN () % GetNumNodes () != 0, "There should be equal number of UEs per eNB. Total eNBs = " << GetNumNodes ());
  NS_ABORT_MSG_IF (m_minDistance > m_d / 2, "UE minimum distance from eNB must be less than ISD/2 = " << m_d / 2);

  //Storage for return value
  NetDeviceContainer ueDevs;

  uint32_t numUesPerSector = ues.GetN () / GetNumNodes ();
  uint32_t numUesHotspot = 2 * numUesPerSector / 3;
  uint32_t currentUe = 0;

  //Get node position and antenna orientation
  std::vector<Vector> eNodeBpos = GetNodePositions ();
  //Loop through each cell
  for (uint32_t n = 0; n < eNodeBpos.size (); n++)
    {
      //position uses z for the antenna direction
      Vector pos = eNodeBpos.at (n);
      double xSector, ySector;
      double xPos, yPos;
      //Compute center of sector hexagon
      //This hexagon has center on cell flat edge
      xSector = pos.x + m_d / 2 * std::cos (DegreesToRadians (pos.z));
      ySector = pos.y + m_d / 2 * std::sin (DegreesToRadians (pos.z));
      NS_LOG_LOGIC ("sector_center " << n << " at " << xSector << ":" << ySector);

      //Randomly select an area within each cell area
      do
        {
          //pick random x and y
          xPos = pos.x + m_uniformRandomVariable->GetValue (-m_d, m_d);
          yPos = pos.y + m_uniformRandomVariable->GetValue (-m_d, m_d);
        }
      while (!(IsInsideHex (pos.x, pos.y, m_d / 2, xPos, yPos, true) && IsInsideHex (xSector, ySector, m_d / 2 * std::cos (DegreesToRadians (30)), xPos, yPos, false))
             || GetDistance (pos.x, pos.y, xPos, yPos) < m_minEnodeb2HotspotDistance
             || !IsAtLeastDistance (xPos, yPos, m_hotspotPositions, m_minHotspotToHotspotDistance));
      // do while( Not(xPos, yPos are in the intersection of flat top hex and a pointy top hex whose center is on hex flat edge)
      // or min distance between hotspot and eNBs not met
      // or min distance to other hotspots not met)
      Vector hotspotPosition (xPos, yPos, 0);
      NS_LOG_LOGIC ("hotspot " << m_hotspotPositions.size () << " at " << hotspotPosition);
      m_hotspotPositions.push_back (hotspotPosition);

      //Randomly and uniformly drop 2/3 UEs within hotspotRange(default 40 m) of the selected area
      for (uint32_t u = 0; u < numUesHotspot; u++)
        {
          do
            {
              //pick random x and y
              xPos = hotspotPosition.x + m_uniformRandomVariable->GetValue (-m_hotspotRange, m_hotspotRange);
              yPos = hotspotPosition.y + m_uniformRandomVariable->GetValue (-m_hotspotRange, m_hotspotRange);
            }
          while (GetDistance (hotspotPosition.x, hotspotPosition.y, xPos, yPos) > m_hotspotRange);
          // do while( distance > m_hotpotRange)

          //assign position to node
          Vector pos (xPos, yPos, m_ueHeight);
          Ptr<Node> node = ues.Get (currentUe);
          Ptr<MobilityModel> mm = node->GetObject<MobilityModel> ();
          NS_LOG_LOGIC ("ue " << u << " at " << pos);
          mm->SetPosition (Vector (xPos, yPos, m_ueHeight));
          ueDevs.Add (m_lteHelper->InstallUeDevice (node));

          //increment UE counter
          currentUe++;
        }
      //Randomly and uniformly drop the remaining 1/3 UEs to the entire macro geographical area of the given macro cell
      for (uint32_t u = numUesHotspot; u < numUesPerSector; u++)
        {
          do
            {
              //pick random x and y
              xPos = pos.x + m_uniformRandomVariable->GetValue (-m_d, m_d);
              yPos = pos.y + m_uniformRandomVariable->GetValue (-m_d, m_d);
            }
          while (!(IsInsideHex (pos.x, pos.y, m_d / 2, xPos, yPos, true) && IsInsideHex (xSector, ySector, m_d / 2 * std::cos (DegreesToRadians (30)), xPos, yPos, false))
                 || GetDistance (pos.x, pos.y, xPos, yPos) < m_minDistance);
          // do while( Not(xPos, yPos are in the intersection of flat top hex and a pointy top hex whose center is on hex flat edge)
          // or distance < m_minDistance)

          //assign position to node
          Vector pos (xPos, yPos, m_ueHeight);
          Ptr<Node> node = ues.Get (currentUe);
          Ptr<MobilityModel> mm = node->GetObject<MobilityModel> ();
          NS_LOG_LOGIC ("ue " << u << " at " << pos);
          mm->SetPosition (Vector (xPos, yPos, m_ueHeight));
          ueDevs.Add (m_lteHelper->InstallUeDevice (node));

          //increment UE counter
          currentUe++;
        }

    }
  return ueDevs;
}

NetDeviceContainer
Lte3gppHexGridEnbTopologyHelper::DropUEsIndoorOutdoor (NodeContainer ues, double w, double l, double h)
{
  NS_LOG_FUNCTION (this);

  //make sure there will be equal number of UEs per sector/cell
  NS_ABORT_MSG_IF (ues.GetN () % GetNumNodes () != 0, "There should be equal number of UEs per eNB. Total eNBs = " << GetNumNodes ());
  NS_ABORT_MSG_IF (h < 0, "height of the building should be greater than 0. Buildings with zero height and underground buildings not supported");

  //Storage for return value
  NetDeviceContainer ueDevs;

  uint32_t numUesPerSector = ues.GetN () / GetNumNodes ();
  uint32_t numIndoorUes =  0.8 * numUesPerSector; // 80% of the UEs are indoor Table A.2.1.1-1
  uint32_t numOutdoorUes = numUesPerSector - numIndoorUes; //remaining 20% of the UEs are outdoor Table A.2.1.1-1
  uint32_t currentUe = 0;
  std::vector< Ptr<Building> > buildings;
  std::vector< Ptr<Building> > buildingsInCurrentCell; //This vector contains the pointers to the buildings of a single cell at a time.

  //Get node position and antenna orientation
  std::vector<Vector> eNodeBpos = GetNodePositions ();
  std::vector<Vector> buildingpos;
  //Loop through each cell
  for (uint32_t n = 0; n < eNodeBpos.size (); n++)
    {
      //position uses z for the antenna direction
      Vector pos = eNodeBpos.at (n);
      double xSector, ySector;
      double xPos, yPos;
      std::vector<Vector> buildingsInCell;

      NS_LOG_DEBUG ("eNB antenna orientation : " << pos.z);

      //Compute center of sector hexagon
      //This hexagon has center on cell flat edge

      xSector = pos.x + m_d / 2 * std::cos (DegreesToRadians (pos.z));
      ySector = pos.y + m_d / 2 * std::sin (DegreesToRadians (pos.z));

      NS_LOG_DEBUG ("sector_center " << n << " at " << xSector << ":" << ySector);

      //Drop 2 RRH buildings within each cell area. Each building is w (m) x l (m) x h (m) , i.e., width x length x height
      for (uint8_t numbuildings = 0; numbuildings < MAX_SUPPORTED_BUILDINGS; numbuildings++)
        {
          do
            {
              //pick random x and y coordinates as the center point of the building
              xPos = pos.x + m_uniformRandomVariable->GetValue (-m_d, m_d);
              yPos = pos.y + m_uniformRandomVariable->GetValue (-m_d, m_d);
            }
          while (!(IsInsideHex (pos.x, pos.y, m_d / 2, xPos, yPos, true) && IsInsideHex (xSector, ySector, m_d / 2 * std::cos (DegreesToRadians (30)), xPos, yPos, false))
                 || GetDistance (pos.x, pos.y, xPos, yPos) < m_minEnodebToBuildingDistance
                 || AreOverlapping (xPos, yPos, w, l, h, buildings));
          // do while( Not(x and y coordinates of the center of a building are in the intersection of flat top hex and a pointy top hex whose center is on hex flat edge)
          //or min distance to this building and eNodeB not meet
          // or the current building overlaps with any of the buildings deployed in the scenario)
          bool Overlap = AreOverlapping (xPos, yPos, w, l, h, buildings);
          NS_LOG_DEBUG ("Building Overlap with other building :" << Overlap);
          buildings.push_back (AddBuilding (xPos, yPos, w, l, h));
          buildingsInCurrentCell.push_back (buildings[buildings.size () - 1]);
        }


      //Randomly drop 80% UEs within buildings
      std::vector<Vector> indoorUes;
      NS_LOG_DEBUG ("Number of Indoor UEs : " << numIndoorUes);
      for (uint32_t u = 0; u < numIndoorUes; u++)
        {
          //randomly select the building to drop the UE
          uint32_t buildingIndex = m_uniformRandomVariable->GetValue (0, 2) > 1 ? 1 : 0;
          NS_LOG_LOGIC ("Deploying UE in building " << buildingIndex);
          Box box = buildingsInCurrentCell[buildingIndex]->GetBoundaries ();
          Ptr<Building> selectedBuilding = buildingsInCurrentCell[buildingIndex];
          NS_LOG_DEBUG ("Deploying UE in building " << buildingIndex << " with dimensions : " << box.xMin << ":" << box.xMax << ":" << box.yMin << ":" << box.yMax << ":" << box.zMin << ":" << box.zMax);

          do
            {
              //pick random x and y within the range of building dimension
              xPos = m_uniformRandomVariable->GetValue (box.xMin, box.xMax);
              yPos = m_uniformRandomVariable->GetValue (box.yMin, box.yMax);
            }
          while (!(IsInsideHex (pos.x, pos.y, m_d / 2, xPos, yPos, true) && IsInsideHex (xSector, ySector, m_d / 2 * std::cos (DegreesToRadians (30)), xPos, yPos, false)
                   && IsAtLeastDistance (xPos, yPos, indoorUes, m_minUeToUeDistanceInBuilding) && IsInsideBuilding (xPos, yPos, selectedBuilding)));
          // do while(Not(xPos, yPos are in the intersection of flat top hex and a pointy top hex whose center is on hex flat edge
          //&& UE to UE minimum distance inside the building is satisfied
          //&& UE is inside the selected building))

          NS_LOG_DEBUG ("Is UE inside the selected building : " << IsInsideBuilding (xPos, yPos, selectedBuilding));
          //assign position to node
          Vector pos (xPos, yPos, m_ueHeight);
          Ptr<Node> node = ues.Get (currentUe);
          Ptr<MobilityModel> mm = node->GetObject<MobilityModel> ();
          NS_LOG_LOGIC ("UE " << u << " at " << pos);
          mm->SetPosition (Vector (xPos, yPos, m_ueHeight));
          ueDevs.Add (m_lteHelper->InstallUeDevice (node));
          indoorUes.push_back (pos);

          //increment UE counter
          currentUe++;
        }

      Ptr<Building> buildingOne = buildingsInCurrentCell[0];
      Ptr<Building> buildingTwo = buildingsInCurrentCell[1];
      NS_LOG_DEBUG ("Number of outdoor UEs : " << numOutdoorUes);
      //Randomly and uniformly drop the remaining 20% UEs outside in the entire macro geographical area of the given macro cell
      for (uint32_t u = 0; u < numOutdoorUes; u++)
        {
          do
            {
              //pick random x and y
              xPos = pos.x + m_uniformRandomVariable->GetValue (-m_d, m_d);
              yPos = pos.y + m_uniformRandomVariable->GetValue (-m_d, m_d);
            }
          while (!(IsInsideHex (pos.x, pos.y, m_d / 2, xPos, yPos, true) && IsInsideHex (xSector, ySector, m_d / 2 * std::cos (DegreesToRadians (30)), xPos, yPos, false))
                 || GetDistance (pos.x, pos.y, xPos, yPos) < m_minDistance
                 || ((IsInsideBuilding (xPos, yPos, buildingOne)|| IsInsideBuilding (xPos, yPos, buildingTwo)))
                 );
          // do while(Not(xPos, yPos in the intersection of flat top hex and a pointy top hex whose center is on hex flat edge)
          //or distance < m_minDistance
          //or dropped in any of the two building)

          NS_LOG_DEBUG ("Deploying UE : " << currentUe << " outside");
          //assign position to node
          Vector pos (xPos, yPos, m_ueHeight);
          Ptr<Node> node = ues.Get (currentUe);
          Ptr<MobilityModel> mm = node->GetObject<MobilityModel> ();
          NS_LOG_LOGIC ("UE " << currentUe << " at " << pos);
          mm->SetPosition (Vector (xPos, yPos, m_ueHeight));
          ueDevs.Add (m_lteHelper->InstallUeDevice (node));

          //increment UE counter
          currentUe++;
        }
      NS_LOG_LOGIC ("Total Indoor UEs for this sector/cell = " << indoorUes.size () << " ratio = " << 100 * ((double)indoorUes.size () / numUesPerSector) << "%");
      NS_LOG_LOGIC ("Total buildings in this sector/cell = " << buildingsInCurrentCell.size ());
      buildingsInCurrentCell.erase (buildingsInCurrentCell.begin (), buildingsInCurrentCell.end ()); //we erase already deployed buildings before moving to a new sector/cell
    }

  return ueDevs;
}


uint32_t
Lte3gppHexGridEnbTopologyHelper::GetNumNodes ()
{
  NS_ASSERT (m_numRings > 0);

  uint32_t numNodes = 1;

  for (uint32_t i = 1; i < m_numRings; i++)
    {
      numNodes += i * 6;
    }

  return 3 * numNodes;
}

uint32_t
Lte3gppHexGridEnbTopologyHelper::GetNumRings () const
{
  NS_LOG_FUNCTION (this);
  return m_numRings;
}


bool
Lte3gppHexGridEnbTopologyHelper::IsInsideHex (double xCenter, double yCenter, double h, double xPos, double yPos, bool flatBottom)
{

  double q2x = std::abs (xPos - xCenter);         // transform the test point locally and to quadrant 2
  double q2y = std::abs (yPos - yCenter);         // transform the test point locally and to quadrant 2

  if (flatBottom)
    {
      double v = h;
      h = v / std::sqrt (3);
      //New conditions because our cell site hex is rotated 90 degrees, having a bottom flat side edge
      if (q2x > 2 * h || q2y > v)
        {
          return false;                                   // bounding test (since q2 is in quadrant 2 only 2 tests are needed)
        }
      return 2 * v * h - v * q2x - h * q2y >= 0;
    }
  else
    {
      double v = h / std::sqrt (3);
      if (q2x > h || q2y > v * 2)
        {
          return false;                                   // bounding test (since q2 is in quadrant 2 only 2 tests are needed)
        }
      return 2 * v * h - v * q2x - h * q2y >= 0;     // finally the dot product can be reduced to this due to the hexagon symmetry
    }
}

double
Lte3gppHexGridEnbTopologyHelper::GetDistance (double x1, double y1, double x2, double y2)
{
  return sqrt ((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
}

bool
Lte3gppHexGridEnbTopologyHelper::IsWithinDistance (double x, double y, std::vector<Vector> positions, double d)
{
  bool ok = true;
  for (uint32_t n = 0; n < positions.size (); n++)
    {
      Vector pos = positions.at (n);
      if (GetDistance (x, y, pos.x, pos.y) > d)
        { //at least one distance is too long
          ok = false;
          break;
        }
    }
  return ok;
}

bool
Lte3gppHexGridEnbTopologyHelper::IsAtLeastDistance (double x, double y, std::vector<Vector> positions, double d)
{
  bool ok = true;
  for (uint32_t n = 0; n < positions.size (); n++)
    {
      Vector pos = positions.at (n);
      if (GetDistance (x, y, pos.x, pos.y) < d)
        { //at least one distance is too short
          ok = false;
          break;
        }
    }
  return ok;
}

bool
Lte3gppHexGridEnbTopologyHelper::IsInsideBuilding (double x, double y, Ptr<Building> building)
{
  Box box = building->GetBoundaries ();

  NS_ABORT_MSG_IF ((m_ueHeight<box.zMin || m_ueHeight>box.zMax), "UE height outside the range of building height");
  Vector pos (x, y, m_ueHeight);
  return building->IsInside (pos);
}

bool
Lte3gppHexGridEnbTopologyHelper::AreOverlapping (double x, double y, double w, double l, double h, std::vector< Ptr<Building> > buildings)
{
  bool overlap = false;
  Box currentBuilding = Box (x - w / 2, x + w / 2, y - l / 2, y + l / 2, 0.0, h);
  Box b;
  for (const auto &it : buildings)
    {
      b = it->GetBoundaries ();
      if (!((currentBuilding.xMin > b.xMax) || (b.xMin > currentBuilding.xMax) || (currentBuilding.yMin > b.yMax) || (b.yMin > currentBuilding.yMax)))
        {
          overlap = true;
          break;
        }
    }
  return overlap;
}

Vector
Lte3gppHexGridEnbTopologyHelper::GetClosestPositionInWrapAround (Vector txPos, Vector rxPos)
{
  NS_LOG_FUNCTION (this << txPos << rxPos);

  NS_ABORT_MSG_IF (m_numRings > 4 || m_numRings < 0, "WRAP-AROUND : NO SUPPORT FOR TOPOLOGIES OTHER THAN 1 THROUGH 4 RINGS!");

  //For LTE scenario, compute the distance between the UE (txPos) and the eNB (rxPos) in the central cluster along with its
  //6 wrap-around locations. After that, return the position of an eNB with the shortest distance to UE.

  //For D2D scenario, compute the distance between the transmitter UE (txPos) and the receiver UE (rxPos) along with its
  //6 wrap around locations. And then, return the position of the receiver UE with the shortest distance to transmitter UE.

  double s = m_d / std::sqrt (3); //hexagon side length
  Vector minPos (rxPos.x, rxPos.y, rxPos.z);
  double minD = GetDistance (txPos.x, txPos.y, rxPos.x, rxPos.y);
  NS_LOG_DEBUG ("C1 pos " << minPos << " distance=" << minD);
  NS_LOG_DEBUG ("C1 center " << rxPos.x << ":" << rxPos.y);
  m_enbPositionsCCluster.push_back (rxPos.x);
  m_enbPositionsCCluster.push_back (rxPos.y);
  //In LTE scenario, rxPos.z is the antenna orientation of the eNB and is different for all the three eNBs of three sector site.
  //To avoid three different keys of the map for the same sight we insert m_siteHeight as z coordinate of the eNB.
  if (rxPos.z == m_ueHeight)
    {
      m_enbPositionsCCluster.push_back (m_ueHeight);
    }
  else
    {
      m_enbPositionsCCluster.push_back (m_siteHeight);
    }

  Vector pos (rxPos.x + (1.5 * ((2 * m_numRings) - 1)) * s, rxPos.y + 0.5 * m_d, rxPos.z);
  double d = GetDistance (txPos.x, txPos.y, pos.x, pos.y);
  NS_LOG_DEBUG ("C2 pos " << pos << " distance=" << d);
  NS_LOG_DEBUG ("C2 center " << pos.x << ":" << pos.y);
  m_wAroundReplica.push_back (Vector (pos.x, pos.y, rxPos.z));

  if (d < minD)
    {
      minD = d;
      minPos = pos;
    }

  pos.x = rxPos.x + (1.5 * (m_numRings - 1)) * s;
  pos.y = rxPos.y + (1 + (1.5 * (m_numRings - 1))) * m_d;
  d = GetDistance (txPos.x, txPos.y, pos.x, pos.y);
  NS_LOG_DEBUG ("C3 pos " << pos << " distance=" << d);
  NS_LOG_DEBUG ("C3 center " << pos.x << ":" << pos.y);
  m_wAroundReplica.push_back (Vector (pos.x, pos.y, rxPos.z));

  if (d < minD)
    {
      minD = d;
      minPos = pos;
    }

  pos.x = rxPos.x - (1.5 * m_numRings) * s;
  pos.y = rxPos.y + (0.5 + (1.5 * (m_numRings - 1))) * m_d;
  d = GetDistance (txPos.x, txPos.y, pos.x, pos.y);
  NS_LOG_DEBUG ("C4 pos " << pos << " distance=" << d);
  NS_LOG_DEBUG ("C4 center " << pos.x << ":" << pos.y);
  m_wAroundReplica.push_back (Vector (pos.x, pos.y, rxPos.z));

  if (d < minD)
    {
      minD = d;
      minPos = pos;
    }

  pos.x = rxPos.x - (1.5 * ((2 * m_numRings) - 1)) * s;
  pos.y = rxPos.y - 0.5 * m_d;
  d = GetDistance (txPos.x, txPos.y, pos.x, pos.y);
  NS_LOG_DEBUG ("C5 pos " << pos << " distance=" << d);
  NS_LOG_DEBUG ("C5 center " << pos.x << ":" << pos.y);
  m_wAroundReplica.push_back (Vector (pos.x, pos.y, rxPos.z));

  if (d < minD)
    {
      minD = d;
      minPos = pos;
    }

  pos.x = rxPos.x - (1.5 * (m_numRings - 1)) * s;
  pos.y = rxPos.y - (1 + (1.5 * (m_numRings - 1))) * m_d;
  d = GetDistance (txPos.x, txPos.y, pos.x, pos.y);
  NS_LOG_DEBUG ("C6 pos " << pos << " distance=" << d);
  NS_LOG_DEBUG ("C6 center " << pos.x << ":" << pos.y);
  m_wAroundReplica.push_back (Vector (pos.x, pos.y, rxPos.z));

  if (d < minD)
    {
      minD = d;
      minPos = pos;
    }

  pos.x = rxPos.x + (1.5 * m_numRings) * s;
  pos.y = rxPos.y - (0.5 + (1.5 * (m_numRings - 1))) * m_d;
  d = GetDistance (txPos.x, txPos.y, pos.x, pos.y);
  NS_LOG_DEBUG ("C7 pos " << pos << " distance=" << d);
  NS_LOG_DEBUG ("C7 center " << pos.x << ":" << pos.y);
  m_wAroundReplica.push_back (Vector (pos.x, pos.y, rxPos.z));

  if (d < minD)
    {
      minD = d;
      minPos = pos;
    }

  WrapAroundReplicas replicas;
  replicas.positions = m_wAroundReplica;
  std::pair<std::map<std::vector<double>,WrapAroundReplicas >::iterator,bool> insertion = m_mapForWAround.insert (std::make_pair (m_enbPositionsCCluster, replicas));
  m_enbPositionsCCluster.erase (m_enbPositionsCCluster.begin (),m_enbPositionsCCluster.end ());
  m_wAroundReplica.erase (m_wAroundReplica.begin (), m_wAroundReplica.end ());

  if (insertion.second)
    {
      NS_LOG_DEBUG ("Inserting the wrap-around replicas of hex positioned at " << m_enbPositionsCCluster[0] << " , " << m_enbPositionsCCluster[1]);
    }
  else
    {
      NS_LOG_DEBUG ("Insertion failed. The wrap-around replicas of hex positioned at " << m_enbPositionsCCluster[0] << " , " << m_enbPositionsCCluster[1] << "already exist in the map");
    }

  NS_LOG_DEBUG ("Pos for min distance " << minPos << " distance=" << minD);

  return minPos;
}

std::vector< Ptr<Building> >
Lte3gppHexGridEnbTopologyHelper::InstallWrapAroundBuildings (std::vector< Ptr<Building> > buildings)
{
  NS_ABORT_MSG_IF (m_numRings > 4 || m_numRings < 0, "WRAP-AROUND : NO SUPPORT FOR TOPOLOGIES OTHER THAN 1 THROUGH 4 RINGS!");
  double s = m_d / std::sqrt (3); //hexagon side length

  std::vector< Ptr<Building> > allBuildings;
  std::vector< Ptr<Building> >::iterator it;
  for (it = buildings.begin (); it != buildings.end (); it++)
    {
      Box b = (*it)->GetBoundaries ();
      //add current building and 6 other copies in extended coverage
      allBuildings.push_back (*it); // no need to duplicate the building
      allBuildings.push_back (AddBuilding (b.xMin + (1.5 * ((2 * m_numRings) - 1)) * s, b.xMax + (1.5 * ((2 * m_numRings) - 1)) * s, b.yMin + 0.5 * m_d, b.yMax + 0.5 * m_d, b.zMin, b.zMax));
      allBuildings.push_back (AddBuilding (b.xMin + (1.5 * (m_numRings - 1)) * s, b.xMax + (1.5 * (m_numRings - 1)) * s, b.yMin + (1 + (1.5 * (m_numRings - 1))) * m_d, b.yMax + (1 + (1.5 * (m_numRings - 1))) * m_d, b.zMin, b.zMax));
      allBuildings.push_back (AddBuilding (b.xMin - (1.5 * m_numRings) * s, b.xMax - (1.5 * m_numRings) * s, b.yMin + (0.5 + (1.5 * (m_numRings - 1))) * m_d, b.yMax + (0.5 + (1.5 * (m_numRings - 1))) * m_d, b.zMin, b.zMax));
      allBuildings.push_back (AddBuilding (b.xMin - (1.5 * ((2 * m_numRings) - 1)) * s, b.xMax - (1.5 * ((2 * m_numRings) - 1)) * s, b.yMin - 0.5 * m_d, b.yMax - 0.5 * m_d, b.zMin, b.zMax));
      allBuildings.push_back (AddBuilding (b.xMin - (1.5 * (m_numRings - 1)) * s, b.xMax - (1.5 * (m_numRings - 1)) * s, b.yMin - (1 + (1.5 * (m_numRings - 1))) * m_d, b.yMax - (1 + (1.5 * (m_numRings - 1))) * m_d, b.zMin, b.zMax));
      allBuildings.push_back (AddBuilding (b.xMin + (1.5 * m_numRings) * s, b.xMax + (1.5 * m_numRings) * s, b.yMin - (0.5 + (1.5 * (m_numRings - 1))) * m_d, b.yMax - (0.5 + (1.5 * (m_numRings - 1))) * m_d, b.zMin, b.zMax));
    }
  return allBuildings;
}

void
Lte3gppHexGridEnbTopologyHelper::AttachWithWrapAround (Ptr<PropagationLossModel> lossModel, NetDeviceContainer ueDevices, NetDeviceContainer enbDevices)
{
  NS_ABORT_MSG_IF (m_numRings > 4 || m_numRings < 0, "WRAP-AROUND : NO SUPPORT FOR TOPOLOGIES OTHER THAN 1 THROUGH 4 RINGS!");
  Vector closestPos;
  Vector bestClosestPos;
  Ptr<NetDevice> enbDev;
  Ptr<NetDevice> bestEnbDev;
  Ptr<NetDevice> ueDev;
  uint32_t bestEnb = 0;
  double rsrp = 0;
  double bestRsrp;
  double txPower = 0;
  Vector posUe;
  Vector posEnb;
  Ptr<LteUeNetDevice> lteUeDev;
  Ptr<LteEnbNetDevice> lteEnbDev;
  Ptr<LteEnbNetDevice> bestLteEnbDev;

  NS_LOG_DEBUG ("Number of UEs : " << ueDevices.GetN ());
  NS_LOG_DEBUG ("Number of eNBs : " << enbDevices.GetN ());

  for (uint32_t u = 0; u < ueDevices.GetN (); ++u)
    {
      bestRsrp = -std::numeric_limits<double>::infinity ();
      ueDev = ueDevices.Get (u);
      posUe = ueDev->GetNode ()->GetObject<MobilityModel> ()->GetPosition ();

      for (uint32_t i = 0; i < enbDevices.GetN (); ++i)
        {

          enbDev = enbDevices.Get (i);
          //Save the position of the eNB to restore it after RSRP is calculated for its closest position including its wrap-around locations
          posEnb = enbDev->GetNode ()->GetObject<MobilityModel> ()->GetPosition ();
          //Find position of the eNB closest to the UE (out of the eNB and its six wrap-around locations)
          closestPos = GetClosestPositionInWrapAround (posUe,posEnb);
          //Change the position of the eNB and set location to the found closest eNB
          //to act as an eNB in a wrap-around location, so RSRP can be calculated
          enbDev->GetNode ()->GetObject<MobilityModel> ()->SetPosition (closestPos);
          lteEnbDev = DynamicCast<LteEnbNetDevice> (enbDev);
          NS_ABORT_MSG_IF (lteEnbDev == 0, "No LteEnbNetDevice found");
          Ptr<LteEnbPhy> lteEnbPhy = lteEnbDev->GetPhy ();
          NS_ABORT_MSG_IF (lteEnbPhy == 0, "No LteEnbPhy found");
          txPower = lteEnbPhy->GetTxPower ();
          NS_LOG_DEBUG ("Txpower : " << txPower);
          Ptr<SpectrumPhy> txPhy = lteEnbPhy->GetDlSpectrumPhy ();


          lteUeDev = DynamicCast<LteUeNetDevice> (ueDev);
          NS_ABORT_MSG_IF (lteUeDev == 0, "No LteUeNetDevice found");
          Ptr<LteUePhy> lteUePhy = lteUeDev->GetPhy ();
          NS_ABORT_MSG_IF (lteUePhy == 0, "No LteUePhy found");
          Ptr<SpectrumPhy> rxPhy = lteUePhy->GetUlSpectrumPhy ();

          //Calculate RSRP from eNB to UE
          rsrp = SidelinkRsrpCalculator::CalcSlRsrpTxPw (lossModel, txPower, txPhy, rxPhy);

          NS_LOG_DEBUG ("UE : " << u << " IMSI : " << lteUeDev->GetImsi () << " to Cell id : " << lteEnbDev->GetCellId ()
                                << " Position of closest eNB : " << lteEnbDev->GetNode ()->GetObject<MobilityModel> ()->GetPosition () << " RSRP : " << rsrp << " dBm");


          if (bestRsrp < rsrp)
            {
              bestRsrp = rsrp;
              bestEnb = i;
              bestClosestPos = closestPos;
            }
          //Restore eNB position, since we already cache the pathloss restoring the position will not affect the future pathloss values.
          //This is needed to keep intact the position of the central cluster so that the remaining UEs can find the closest position correctly.
          enbDev->GetNode ()->GetObject<MobilityModel> ()->SetPosition (posEnb);
        }


      bestEnbDev = enbDevices.Get (bestEnb);
      m_lteHelper->Attach (ueDev,bestEnbDev); //attach

      bestLteEnbDev = DynamicCast<LteEnbNetDevice> (bestEnbDev);

      NS_LOG_DEBUG ("Attached UE with IMSI : " << lteUeDev->GetImsi () << " to Cell id : " << bestLteEnbDev->GetCellId ()
                                               << " Best eNB position  : " << bestClosestPos << " RSRP = " << bestRsrp);

      if (IsInWrapAround (bestClosestPos.x, bestClosestPos.y))
        {
          NS_LOG_DEBUG ("Attached in wrap-around. UE : " << u << " IMSI : " << lteUeDev->GetImsi () << " to Cell id : " << bestLteEnbDev->GetCellId ()
                                                         << " Position of best eNB in central cluster : " << bestEnbDev->GetNode ()->GetObject<MobilityModel> ()->GetPosition ()
                                                         << " Best eNB position in wrap-around : " << bestClosestPos);
          m_imsi = lteUeDev->GetImsi ();
          m_wrapAroundInfo.cellId = bestLteEnbDev->GetCellId ();
          m_wrapAroundInfo.posUe = posUe;
          m_wrapAroundInfo.posEnb = bestClosestPos;
          m_wrapAroundInfo.rsrp = bestRsrp;
          m_mapForWrapAroundInfo.insert (std::make_pair (m_imsi,m_wrapAroundInfo));
        }

    }

}

Ptr<Building>
Lte3gppHexGridEnbTopologyHelper::AddBuilding (double x, double y, double w, double l, double h)
{
  NS_LOG_DEBUG ("Building dimensions " << x - w / 2 << " : " << x + w / 2 << " : " << y - l / 2 << " : " << y + l / 2 << " : " << 0.0 << " : " << h);
  Ptr<Building> building = AddBuilding (x - w / 2, x + w / 2, y - l / 2, y + l / 2, 0.0, h);
  return building;
}

Ptr<Building>
Lte3gppHexGridEnbTopologyHelper::AddBuilding (double xMin, double xMax, double yMin, double yMax, double zMin, double zMax)
{
  Ptr<Building> building = CreateObject<Building> ();
  building->SetBoundaries (Box (xMin, xMax, yMin, yMax, zMin, zMax));
  building->SetBuildingType (Building::Office);
  building->SetExtWallsType (Building::ConcreteWithWindows);
  building->SetNFloors (1);
  return building;
}

std::vector<Vector>
Lte3gppHexGridEnbTopologyHelper::GetHotSpotPositions ()
{
  NS_LOG_FUNCTION (this);
  return m_hotspotPositions;
}

int64_t
Lte3gppHexGridEnbTopologyHelper::AssignStreams (int64_t stream)
{
  NS_LOG_FUNCTION (this << stream);
  m_uniformRandomVariable->SetStream (stream);
  return 1;
}

std::map<std::vector<double>,WrapAroundReplicas>
Lte3gppHexGridEnbTopologyHelper:: GetWrapAroundPositions ()
{
  NS_LOG_FUNCTION (this);
  return m_mapForWAround;
}

std::map<uint64_t,WrapAroundInfo_t>
Lte3gppHexGridEnbTopologyHelper::GetWrapAroundAttachInfo ()
{
  NS_LOG_FUNCTION (this);
  return m_mapForWrapAroundInfo;
}

bool
Lte3gppHexGridEnbTopologyHelper::IsInWrapAround (double x, double y)
{
  NS_LOG_FUNCTION (this << x << y);

  bool isInWrapAround = false;

  double interSiteDistance = m_d;
  uint32_t numRings = GetNumRings ();
  NS_LOG_DEBUG ("numRings " << numRings);
  double s = interSiteDistance / std::sqrt (3); //hexagon side length
  double xCenterLastRing = 0;
  double xMaxLastRing = 0;
  double yMaxLastRing = 0;

  uint32_t moreThanOneRings = numRings > 1 ? 1 : 0;
  xCenterLastRing = (3 * s / 2) * (numRings - 1);
  xMaxLastRing = s + xCenterLastRing  * moreThanOneRings;
  yMaxLastRing = interSiteDistance * (numRings - 1) + interSiteDistance / 2;

  NS_LOG_DEBUG ("xMinLastRing " << -xMaxLastRing << " xMaxLastRing " << xMaxLastRing << " yMinLastRing " << -yMaxLastRing << " yMaxLastRing " << yMaxLastRing);

  //Before checking every hexagon of central cluster do a quick check to detect if the point is outside max limits.
  if (x > xMaxLastRing || x < -xMaxLastRing)
    {
      NS_LOG_DEBUG ("Point is in wrap-around");
      isInWrapAround = true;
      return isInWrapAround;
    }
  else if (y > yMaxLastRing || y < -yMaxLastRing)
    {
      NS_LOG_DEBUG ("Point is in wrap-around");
      isInWrapAround = true;
      return isInWrapAround;
    }

//if we reach here that means the point is in one of the corner hex whose center is under the limits of +-xMaxLastRing,+-yMaxLastRing but still
//is out of central cluster. For 2 rings, there will 4 such hexagon, each on one corner and for 3 and 4 rings there will be 8 and 12, respectively.
//In any case, this function is general enough to detect any point outside the hexagons of central cluster.

  std::vector<Vector> nodePositions = GetNodePositions ();

  for (const auto &it : nodePositions)
    {
      if (!(IsInsideHex (it.x, it.y, interSiteDistance / 2, x, y, true)))
        {
          NS_LOG_DEBUG ("Point is in wrap-around. Hex center : " << it.x << " : " << it.y);
          isInWrapAround = true;
          NS_LOG_DEBUG ("Check remaining hexagons if any");
        }
      else
        {
          NS_LOG_DEBUG ("Point is inside of the central cluster hexagon. Hex center : " << it.x << " : " << it.y);
          isInWrapAround = false;
          break;
        }
    }

  return isInWrapAround;
}


} // namespace ns3

