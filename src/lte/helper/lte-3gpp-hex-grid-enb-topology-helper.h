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

#ifndef LTE_3GPP_HEX_GRID_ENB_TOPOLOGY_HELPER_H
#define LTE_3GPP_HEX_GRID_ENB_TOPOLOGY_HELPER_H

#include <ns3/lte-helper.h>
#include <ns3/sidelink-rsrp-calculator.h>
#include <ns3/buildings-helper.h>
#include <map>
#include <set>

namespace ns3 {


/**
 * This helper class allows to easily create a topology with eNBs
 * grouped in three-sector sites laid out on an hexagonal grid. The
 * layout is done in a ring fashion such that a center site is
 * surrounded by other sites. We note that a sector is equivalent
 * to a cell. For more information please refer to ITU-R M.2135.
 * For details related to wrap-around topology please refer to
 * http://www.ieee802.org/20/Contribs/C802.20-05-15.doc and
 * http://ieee802.org/20/Contribs/C802.20-03-94.pdf
 *
 */

static const uint8_t MAX_SUPPORTED_BUILDINGS = 2;

/**
 * struct for storing info of all the UEs and the eNBs attached in wrap-around
 * The Cell id of the eNB or IMSI of the UE in case of Sidelink
 * The Position of the UE
 * The Position of the eNB or a Sidelink UE in wrap-around
 * The RSRP in dBm
 */
struct WrapAroundInfo_t
{
  uint64_t cellId; ///< The cell id
  Vector posUe; ///< Position of the UE
  Vector posEnb; ///< Position of the eNB
  double rsrp; ///< RSRP value
};

/**
 * struct for storing the positions of
 * all the six replicas of a hexagon
 * in wrap-around
 */
struct WrapAroundReplicas
{
  std::vector<Vector> positions; ///< A vector to store the coordinates of all the six replicas
};

class Lte3gppHexGridEnbTopologyHelper : public Object
{
public:
  Lte3gppHexGridEnbTopologyHelper (void);
  virtual ~Lte3gppHexGridEnbTopologyHelper (void);

  // inherited from Object
  /**
   *  Register this type.
   *  \return The object TypeId.
   */
  static TypeId GetTypeId (void);
  virtual void DoDispose (void);


  /**
   * Set the LteHelper to be used to actually create the EnbNetDevices and UeNetDevices in hex grid
   *
   * \param h A pointer to the LteHelper to be used
   */
  void SetLteHelper (Ptr<LteHelper> h);

  /**
   * Position the nodes on a hex grid and install the corresponding
   * EnbNetDevices with antenna boresight configured properly as per
   * system simulation baseline parameters in 3GPP TR 36.814 and
   * TR 25.814
   *
   * \param c The node container where the devices are to be installed
   *
   * \return The NetDeviceContainer with the newly created devices
   */
  NetDeviceContainer SetPositionAndInstallEnbDevice (NodeContainer c);

  /**
   * Drops UEs uniformly (i.e., each sector will have equal number of UEs) within the coverage of the three sector cells,
   * leaving no cell gaps, and not exceeding cell boundaries. This method also makes sure that the distance between the eNB
   * and the UE is greater the minimum configured distance (i.e., the value set through the attribute "MinDistance" or
   * directly calling SetMinimumDistance.
   *
   * \param ues The list of UEs
   *
   * \return The NetDeviceContainer with the newly created devices
   */
  NetDeviceContainer DropUEsUniformlyPerSector (NodeContainer ues);

  /**
   * Drops UEs by creating one hotspot per sector/cell (TR 36.843 Table A.2.1.1-1).
   * For each cell, it chooses a random position within the
   * cell range, such that the configured minimum eNB to hotspot distance
   * and minimum hotspot to hotspot distance is maintained. For each
   * sector/cell, 2/3 of its total number of UEs are dropped at
   * a position, which is randomly and uniformly chosen within
   * a configured hotspot range. Remaining 1/3 of the UEs, are dropped at
   * a position, which is randomly and uniformly chosen within
   * the cell range.
   *
   * \param ues The list of UEs
   *
   * \return The NetDeviceContainer with the newly created devices
   */
  NetDeviceContainer DropUEsHotspot (NodeContainer ues);

  /**
   * Drops UEs indoor and outdoor (TR 36.843 Table A.2.1.1-1).
   * Two building per sector/cell are deployed such that the
   * distance between the center of the building and the eNB is
   * greater than m_minEnodebToBuildingDistance. For each sector/cell,
   * 80% of its total number of UEs are dropped inside any of the two
   * deployed buildings at a position, which is randomly and uniformly
   * chosen and also fulfills the minimum UE to UE distance inside a
   * building constraint. Remaining 20% of the UEs, are dropped outside
   * the buildings at a position,which is randomly and uniformly chosen
   * within the cell range.
   *
   * \param ues The list of UEs
   * \param w The width of each building
   * \param l The length of each building
   * \param h The height of each building
   *
   * \return The NetDeviceContainer with the newly created devices
   */
  NetDeviceContainer DropUEsIndoorOutdoor (NodeContainer ues, double w, double l, double h);

  /**
   * Computes the number of nodes needed to create a hex grid as per the
   * number of rings set by using the attribute "NumberOfRings" or directly calling SetNumRings
   * method. E.g, 1 ring contains 3 nodes, 2 rings have 21 nodes, 3 rings have 57 nodes. By default, the
   * function will return the value 3, as the default number of rings is set to 1.
   *
   * \return The number of nodes to be created
   */
  uint32_t GetNumNodes ();

  /**
   * Sets the number of ring in the hexagonal grid
   * There is no limit on the number of rings in a topology without wrap-around.
   * The topology with wrap-around only supports up to 4 rings.
   *
   * \param ring The number of ring (1 means a single site)
   */
  void SetNumRings (uint32_t ring);

  /**
   * Set the coordinates for the center of the grid
   *
   * \param x The x coordinates in m
   * \param y The y coordinates in m
   */
  void SetCenter (double x, double y);

  /**
   * Set the distance between sites
   *
   * \param d The distance in m
   */
  void SetInterSiteDistance (double d);

  /**
   * Set the site height
   *
   * \param height The site height in m
   */
  void SetSiteHeight (double height);

  /**
   * Set the UE height
   *
   * \param height The UE height in m
   */
  void SetUeHeight (double height);

  /**
   * Set the minimum distance between eNodeB and UE.
   * The methods provided to drop UEs take in to account
   * this distance while assigning position to a UE.
   *
   * \param m The minimum distance in meter
   */
  void SetMinimumDistance (double m);

  /**
   * Returns the position of the closest replica of the current cell in wrap-around
   * topology with respect to receiver's position.
   *
   * \param txPos The position of the transmitter
   * \param rxPos The position of the receiver
   *
   * \return The position of the closest replica of the current cell in wrap-around
   * topology
   */
  Vector GetClosestPositionInWrapAround (Vector txPos, Vector rxPos);

  /**
   * Returns a vector containing the positions of all the hotspots created
   * by using the function DropUEsHotspot.
   *
   * \return Position of all the hotspots
   */
  std::vector<Vector> GetHotSpotPositions ();

  /**
   * Installs new buildings in wrap-around locations
   *
   * \param buildings The list of buildings in their normal positions
   *
   * \return The new list of buildings with duplicates in wrap-around positions
   */
  std::vector< Ptr<Building> > InstallWrapAroundBuildings (std::vector< Ptr<Building> > buildings);

  /**
   * Manual attachment of a set of UE devices to the network via the eNodeB with the highest
   * RSRP to that UE. The RSRP is calculated as defined in TR 36.843 for Sidelink (S-RSRP).
   * It uses the wrap around method to find the closest replica of an eNodeB
   * and then calculates the RSRP using the link from those eNodeBs to the UE.
   *
   * \param lossModel A pointer to the propagation loss model to be used
   * \param ueDevices The UE devices to connect
   * \param enbDevices The eNB devices UEs can be connected to
   */
  void AttachWithWrapAround (Ptr<PropagationLossModel> lossModel, NetDeviceContainer ueDevices, NetDeviceContainer enbDevices);


  /**
   * Returns the inter-site distance on the hexagonal grid
   *
   * \return The inter-site distance in m
   */
  double GetInterSiteDistance () const;

  /**
   * Returns the number of ring in the hexagonal grid
   *
   * \return The number of rings (1 means a single site)
   */
  uint32_t GetNumRings () const;

  /**
   * Returns a map containing the position of each hexagon and its six replicas in wrap-around
   * map key value: a vector containing the coordinates of a hexagon in central cluster
   * map mapped value: an object of WrapAroundReplicas, which is a vector containing theç
   * coordinates of all the six replicas
   *
   * \return A map containing the position of each hexagon and its six replicas in wrap-around
   */
  std::map<std::vector <double>,WrapAroundReplicas> GetWrapAroundPositions ();

  /**
   * Returns a map containing the info of all the UEs and the eNBs attached in wrap-around
   * map key value: IMSI of a UE
   * map mapped value: WrapAroundInfo
   *
   * \return A map containing the info of all the UEs and the eNBs attached in wrap-around
   */
  std::map<uint64_t,WrapAroundInfo_t> GetWrapAroundAttachInfo ();

  /**
   * Is in wrap-around function
   * \param x the x coordinate of the point to check
   * \param y the y coordinate of the point to check
   *
   * \return True if a point lie in any of the wrap-around hexagon, False otherwise
   */
  bool IsInWrapAround (double x, double y);

  /**
   * Assign a fixed random variable stream number to the random variables
   * used by this model.  Return the number of streams (possibly zero) that
   * have been assigned.
   *
   * \param stream The first stream index to use
   * \return The number of stream indices assigned by this model
   */
  int64_t AssignStreams (int64_t stream);

private:
  /**
   * Is inside hex function
   *
   * \param xCenter The x coordinate of the hexagon center
   * \param yCenter The y coordinate of the hexagon center
   * \param h The apothem length of the hexagon
   * \param xPos The x coordinate of the point to check
   * \param yPos The y coordinate of the point to check
   * \param flatBottom True if the hexagon have a bottom flat side edge
   *                   False if the hexagon have a pointy top
   *
   * \return True if the given x,y coordinate lie inside the hexagon, False otherwise
   */
  bool IsInsideHex (double xCenter, double yCenter, double h, double xPos, double yPos, bool flatBottom);

  /**
   * Get distance function
   *
   * \param x1 The x coordinate of the first point
   * \param y1 The y coordinate of the first point
   * \param x2 The x coordinate of the second point
   * \param y2 The y coordinate of the second point
   *
   * \return the 2D distance between to points
   */
  double GetDistance (double x1, double y1, double x2, double y2);

  /**
   * Get node positions function
   *
   * \return The position of all the nodes in the central cluster of hexagonal grid
   */
  std::vector<Vector> GetNodePositions ();

  /**
   * Is with in distance function
   *
   * \param x The x coordinate of the point
   * \param y The y coordinate of the point
   * \param pos The vector containing the position of the test points
   * \param d The distance to be verified
   *
   * \return True if the given coordinates are within the distance d of all
   * the other positions in the vector, , False otherwise
   */
  bool IsWithinDistance (double x, double y, std::vector<Vector> pos, double d);

  /**
   * Is atleast distance function
   *
   * \param x The x coordinate of the point
   * \param y The y coordinate of the point
   * \param pos The vector containing the position of the test points
   * \param d The distance to be verified
   *
   * \return True if the given coordinates are at least at the distance d
   * from all the other positions in the vector, , False otherwise
   */
  bool IsAtLeastDistance (double x, double y, std::vector<Vector> pos, double d);

  /**
   * Is inside the building function
   *
   * \param x The x coordinate of the point
   * \param y The y coordinate of the point
   * \param building The pointer to the building
   *
   * \return True if the point lies inside the building, False otherwise
   */
  bool IsInsideBuilding (double x, double y, Ptr<Building> building);

  /**
   * Are overlapping function
   *
   * \param x The x coordinate of the center point of the building
   * \param y The y coordinate of the center point of the building
   * \param w The width of the building
   * \param l The length of the building
   * \param h The height of the building
   * \param buildings The vector containing the pointers to already deployed buildings
   *
   * \return True if a building formed by x,y,w,l,h overlaps with already deployed buildings, False otherwise
   */
  bool AreOverlapping (double x, double y, double w, double l, double h, std::vector< Ptr<Building> > buildings);

  /**
   * Add building function
   * Deploy a building with center coordinates at x,y and the given width,length and height
   *
   * \param x the x coordinate of the center point of the building
   * \param y The y coordinate of the center point of the building
   * \param w The width of the building
   * \param l The length of the building
   * \param h The height of the building
   *
   * \return The pointer to the deployed buildings
   */
  Ptr<Building> AddBuilding (double x, double y, double w, double l, double h);

  /**
   * Add building function
   * Deploy a building with boundaries xMin, xMax, yMin, yMax, zMin, zMax
   *
   * \param xMin The x coordinate of the left boundary of the building
   * \param xMax The x coordinate of the right boundary of the building
   * \param yMin The y coordinate of the left boundary of the building
   * \param yMax The y coordinate of the right boundary of the building
   * \param zMin The z coordinate of the lowest boundary of the building
   * \param zMax The z coordinate of the highest boundary of the building
   *
   * \return The pointer to the deployed buildings
   */
  Ptr<Building> AddBuilding (double xMin, double xMax, double yMin, double yMax, double zMin, double zMax);

  Ptr<LteHelper> m_lteHelper; ///< Pointer to LteHelper
  uint32_t m_numRings; ///< Number of rings
  double m_d; ///< Inter-site distance
  double m_xCenter; ///< The x coordinate of the center of the hex grid
  double m_yCenter; ///< The y coordinate of the center of the hex grid
  double m_siteHeight; ///< The height [m] of each site
  double m_minDistance; ///< The minimum distance between eNodeB and UE
  double m_ueHeight; ///< The height [m] of each UE
  double m_minEnodeb2HotspotDistance; ///< The minimum distance between eNodeB and hotspot
  double m_minHotspotToHotspotDistance; ///< The minimum distance between hotspot centers
  double m_hotspotRange; ///< The radius of the hotspot
  double m_minEnodebToBuildingDistance; ///< The minimum distance between the center of the building and the eNodeB
  double m_minUeToUeDistanceInBuilding; ///< The minimum distance between 2 UEs inside a building
  /**
   * Positions of all the hotspots created. Positions are stored in a way that the first
   * element of this vector contains the position of the hotspot in the range of first eNB,
   * second element contains the position of the hotspot in the range of second eNB and so on.
   */
  std::vector<Vector> m_hotspotPositions;
  Ptr<UniformRandomVariable> m_uniformRandomVariable; ///< Provides uniform random variables.
  std::vector<double> m_enbPositionsCCluster; ///< Stores the position of an eNB in the central cluster of hexagonal grid
  std::vector<Vector> m_wAroundReplica; ///< Stores the coordinates of all the six replicas
  std::map<std::vector<double>,WrapAroundReplicas> m_mapForWAround; ///< A map for storing position of each hexagon and its six replicas in wrap-around
  uint64_t m_imsi; ///< IMSI of the UE attached to an eNB in the wrap-around
  WrapAroundInfo_t m_wrapAroundInfo; ///< structure object to hold the wrap-around info of the UE
  std::map<uint64_t,WrapAroundInfo_t> m_mapForWrapAroundInfo; ///< map to store wrap-around info of the UE
};

} // namespace ns3

#endif // LTE_3GPP_HEX_GRID_ENB_TOPOLOGY_HELPER_H
