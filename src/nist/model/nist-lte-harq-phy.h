/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2012 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Author: Marco Miozzo  <marco.miozzo@cttc.es>
 * Modified by: NIST
 */


#ifndef NIST_LTE_HARQ_PHY_MODULE_H
#define NIST_LTE_HARQ_PHY_MODULE_H



#include <ns3/log.h>
#include <ns3/assert.h>
#include <math.h>
#include <vector>
#include <map>
#include <ns3/simple-ref-count.h>


namespace ns3 {



struct NistHarqProcessInfoElement_t
{
   double m_mi;
   uint8_t m_rv;
   uint16_t m_infoBits;
   uint16_t m_codeBits;
  double m_sinr; //effective mean SINR for the transmission
};

typedef std::vector <NistHarqProcessInfoElement_t> HarqProcessInfoList_t;

/**
 * \ingroup lte
 * \brief The NistLteHarqPhy class implements the HARQ functionalities related to PHY layer
 *(i.e., decodification buffers for incremental redundancy managment)
 *
*/
class NistLteHarqPhy : public SimpleRefCount<NistLteHarqPhy>
{
public:
  NistLteHarqPhy ();
  ~NistLteHarqPhy ();
  
  void SetDiscNumRetx (uint8_t retx);

  void SubframeIndication (uint32_t frameNo, uint32_t subframeNo);

  /**
  * \brief Return the cumulated MI of the HARQ procId in case of retranmissions
  * for DL (asynchronous)
  * \param harqProcId the HARQ proc id
  * \param layer layer no. (for MIMO spatial multiplexing)
  * \return the MI accumulated
  */
  double GetAccumulatedMiDl (uint8_t harqProcId, uint8_t layer);

  /**
  * \brief Return the info of the HARQ procId in case of retranmissions
  * for DL (asynchronous)
  * \param harqProcId the HARQ proc id
  * \param layer layer no. (for MIMO spatail multiplexing)
  * \return the vector of the info related to HARQ proc Id
  */
  HarqProcessInfoList_t GetHarqProcessInfoDl (uint8_t harqProcId, uint8_t layer);

  /**
  * \brief Return the cumulated MI of the HARQ procId in case of retranmissions
  * for UL (synchronous)
  * \return the MI accumulated
  */
  double GetAccumulatedMiUl (uint16_t rnti);

  /**
  * \brief Return the info of the HARQ procId in case of retranmissions
  * for UL (asynchronous)
  * \param rnti the RNTI of the transmitter
  * \param harqProcId the HARQ proc id
  * \return the vector of the info related to HARQ proc Id
  */
  HarqProcessInfoList_t GetHarqProcessInfoUl (uint16_t rnti, uint8_t harqProcId);

  /**
  * \brief Update the Info associated to the decodification of an HARQ process
  * for DL (asynchronous)
  * \param id the HARQ proc id
  * \param layer layer no. (for MIMO spatail multiplexing)
  * \param mi the new MI
  * \param infoBytes the no. of bytes of info
  * \param mi the total no. of bytes txed
  */
  void UpdateDlHarqProcessNistStatus (uint8_t id, uint8_t layer, double mi, uint16_t infoBytes, uint16_t codeBytes);

  /**
  * \brief Update the Info associated to the decodification of an HARQ process
  * for DL (asynchronous)
  * \param id the HARQ proc id
  * \param layer layer no. (for MIMO spatail multiplexing)
  * \param sinr the new SINR
  */
  void UpdateDlHarqProcessNistStatus (uint8_t id, uint8_t layer, double sinr);

  /**
  * \brief Reset  the info associated to the decodification of an HARQ process
  * for DL (asynchronous)
  * \param id the HARQ proc id
  */
  void ResetDlHarqProcessNistStatus(uint8_t id);

  /**
  * \brief Update the MI value associated to the decodification of an HARQ process
  * for UL 
  * \param rnti the RNTI of the transmitter
  * \param mi the new MI
  * \param infoBytes the no. of bytes of info
  * \param mi the total no. of bytes txed
  */
  void UpdateUlHarqProcessNistStatus (uint16_t rnti, double mi, uint16_t infoBytes, uint16_t codeBytes);

  /**
  * \brief Update the mean SINR value associated to the decodification of an HARQ process
  * for UL
  * \param rnti the RNTI of the transmitter
  * \param sinr the new SINR
  */
  void UpdateUlHarqProcessNistStatus (uint16_t rnti, double sinr);

  /**
  * \brief Reset  the info associated to the decodification of an HARQ process
  * for DL (asynchronous)
  * \param id the HARQ proc id
  */
  void ResetUlHarqProcessNistStatus(uint16_t rnti, uint8_t id);
  
  
   /**
  * \brief Return the cumulated MI of the HARQ procId in case of retranmissions
  * for SL
  * \param rnti The UE identifier
  * \param l1dst The layer 1 destination ID 
  * \return the MI accumulated
  */
  double GetAccumulatedMiSl (uint16_t rnti, uint8_t l1dst);

  /**
  * \brief Return the info of the HARQ procId in case of retranmissions
  * for SL 
  * \param rnti the RNTI of the transmitter
  * \param l1dst The layer 1 destination ID 
  * \return the vector of the info related to HARQ proc Id
  */
  HarqProcessInfoList_t GetHarqProcessInfoSl (uint16_t rnti, uint8_t l1dst);
 
  /**
  * \brief Return the info of the HARQ procId in case of retranmissions
  * for d2d discovery
  * \param rnti the RNTI of the transmitter
  * \param resPsdch The resource used
  * \return the vector of the info related to HARQ proc Id
  */
  HarqProcessInfoList_t GetHarqProcessInfoDisc (uint16_t rnti, uint8_t resPsdch);

  /**
  * \brief Update the MI value associated to the decodification of an HARQ process
  * for SL 
  * \param rnti the RNTI of the transmitter
  * \param l1dst the layer 1 destination ID
  * \param mi the new MI
  * \param infoBytes the no. of bytes of info
  * \param mi the total no. of bytes txed
  */
  void UpdateSlHarqProcessNistStatus (uint16_t rnti, uint8_t l1dst, double mi, uint16_t infoBytes, uint16_t codeBytes);

  /**
  * \brief Update the SINR value associated to the decodification of an HARQ process
  * for SL 
  * \param rnti the RNTI of the transmitter
  * \param l1dst the layer 1 destination ID
  * \param sinr the new SINR
  */
  void UpdateSlHarqProcessNistStatus (uint16_t rnti, uint8_t l1dst, double sinr);

  /**
  * \brief Update the MI value associated to the decodification of an HARQ process
  * for DL (asynchronous)
  * \param rnti the RNTI of the transmitter
  * \param resPsdch resource used
  * \param sinr the new SINR
  */
  void UpdateDiscHarqProcessNistStatus (uint16_t rnti, uint8_t resPsdch, double sinr);

  /**
  * \brief Reset  the info associated to the decodification of an HARQ process
  * for SL 
  * \param id the HARQ proc id
  * \param l1dst the layer 1 destination ID
  */
  void ResetSlHarqProcessNistStatus(uint16_t rnti, uint8_t ls1dst);
  
  /**
  * \brief Reset  the info associated to the decodification of an HARQ process
  * \param rnti rnti of the transmitter
  * \param resPsdch resource used
  */
  void ResetDiscHarqProcessNistStatus(uint16_t rnti, uint8_t resPsdch);

private:

  std::vector <std::vector <HarqProcessInfoList_t> > m_miDlHarqProcessesInfoMap;
  std::map <uint16_t, std::vector <HarqProcessInfoList_t> > m_miUlHarqProcessesInfoMap;
  std::map <uint16_t, std::map <uint8_t , HarqProcessInfoList_t> > m_miSlHarqProcessesInfoMap;
  std::map <uint16_t, std::map <uint8_t , HarqProcessInfoList_t> > m_miDiscHarqProcessesInfoMap;  
    
  uint8_t m_discNumRetx;

};


}

#endif /* NIST_LTE_HARQ_PHY_MODULE_H */
