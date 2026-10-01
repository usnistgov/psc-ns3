// Copyright (c) 2019 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
//
// SPDX-License-Identifier: GPL-2.0-only

#include "nr-gnb-mac.h"
#include "nr-spectrum-phy.h"

#include <ns3/event-id.h>
#include <ns3/nstime.h>
#include <ns3/object.h>

#include <functional>

#ifndef NR_CH_ACCESS_MANAGER_H_
#define NR_CH_ACCESS_MANAGER_H_

namespace ns3
{

/**
 * \ingroup nru
 * \brief The Channel Access Manager class
 *
 * This is the interface for any channel access manager. A channel access manager
 * is responsible to listen to the channel, informing the PHY when it is free
 * for transmitting.
 *
 * \section ch_access_manager_req Requesting the channel
 *
 * The PHY would call the method RequestAccess(). Then, when the channel is
 * available for transmission, the channel access manager would call the
 * callback set with the method SetAccessGrantedCallback(). If the channel
 * cannot be access, then the callback set with SetAccessDeniedCallback()
 * will be called, instead. The request can be cancelled by calling Cancel().
 *
 * \section ch_access_manager_conf Configuration
 *
 * Any channel access manager attribute can be set through the helper methods
 * NrHelper::SetUeChannelAccessManagerAttribute() or
 * NrHelper::SetGnbChannelAccessManagerAttribute(). Another option is
 * directly calling `SetAttribute` on the pointer. The list of
 * attributes is reported below, in the Attributes section.
 *
 * \see NrAlwaysOnAccessManager
 */
class NrChAccessManager : public Object
{
  public:
    /**
     * \brief Get the type ID
     * \return the type id
     */
    static TypeId GetTypeId();

    /**
     * \brief ChannelAccessManager constructor
     */
    NrChAccessManager();

    /**
     * \brief ~ChannelAccessManager
     */
    ~NrChAccessManager() override;

    /**
     * \brief Set duration of grant for transmission.
     * \param grantDuration duration of grant
     */
    void SetGrantDuration(Time grantDuration);

    /**
     * \brief Get grant duration time.
     * \returns default grant duration
     */
    Time GetGrantDuration() const;

    /**
     * \brief A function that signal that the channel has been earned
     */
    typedef std::function<void(const Time& time)> AccessGrantedCallback;
    /**
     * \brief A function that signal that the channel is denied and the request should
     * be retried
     */
    typedef std::function<void()> AccessDeniedCallback;

    /**
     * \brief RequestAccess
     *
     * When the channel is granted, the callback set with SetAccessGrantedCallback()
     * will be called.
     */
    virtual void RequestAccess() = 0;
    /**
     * \brief Set Access-Granted Callback
     * \param cb the callback to invoke when the channel access is granted
     */
    virtual void SetAccessGrantedCallback(const AccessGrantedCallback& cb) = 0;
    /**
     * \brief Set Access-Denied Callback
     * \param cb the callback to invoke when the channel access is denied
     */
    virtual void SetAccessDeniedCallback(const AccessDeniedCallback& cb) = 0;

    /**
     * \brief Cancel a previously invoked request for accessing the channel
     */
    virtual void Cancel() = 0;

    /**
     * \brief Set spectrum phy instance for this channel access manager
     * \param spectrumPhy spectrum phy instance
     */
    virtual void SetNrSpectrumPhy(Ptr<NrSpectrumPhy> spectrumPhy);

    /**
     * \brief Getter for spectrum phy instance to which is connected this channel access manager
     * \return pointer to spectrum phy instance
     */
    Ptr<NrSpectrumPhy> GetNrSpectrumPhy();

    /**
     * \brief Set MAC instance for this channel access manager
     * \param mac gNB mac instance
     */
    virtual void SetNrGnbMac(Ptr<NrGnbMac> mac);

    /**
     * \brief Getter for MAC instance to which is connected this channel access manager
     * \return pointer to MAC instance
     */
    Ptr<NrGnbMac> GetNrGnbMac();

  private:
    Time m_grantDuration; //!< Duration of the channel access grant
    Ptr<NrGnbMac> m_mac;  //!< MAC instance to which is connected this channel access manager
    Ptr<NrSpectrumPhy>
        m_spectrumPhy; //!< SpectrumPhy instance to which is connected this channel access manager
};

/**
 * \ingroup nru
 * \brief A Channel access manager that sees the channel always free for transmitting
 *
 * This channel access manager is installed by default in NR instances.
 *
 * \section always_on_usage
 *
 * This is the CAM that is created by default. However, if you want to set it
 * manually, you can invoke the helper function before installing the gnb:
 *
\verbatim
  nrHelper->SetGnbChannelAccessManagerTypeId (NrAlwaysOnAccessManager::GetTypeId());
  ...
  nrHelper->InstallGnb ...
\endverbatim
 *
 * or the UE-side:
\verbatim
  nrHelper->SetUeChannelAccessManagerTypeId (NrAlwaysOnAccessManager::GetTypeId());
  ...
  nrHelper->InstallUe ...
\endverbatim
 *
 * The type of the channel access manager cannot be changed after the helper
 * has installed the UE or the GNB node.
 *
 */
class NrAlwaysOnAccessManager : public NrChAccessManager
{
  public:
    /**
     * \brief Get the type ID
     * \return the type id
     */
    static TypeId GetTypeId();

    /**
     * \brief NrAlwaysOnAccessManager constructor
     */
    NrAlwaysOnAccessManager();
    /**
     * \brief destructor
     */
    ~NrAlwaysOnAccessManager() override;

    // inherited
    void RequestAccess() override;
    void SetAccessGrantedCallback(const AccessGrantedCallback& cb) override;
    void SetAccessDeniedCallback(const AccessDeniedCallback& cb) override;
    void Cancel() override;

  private:
    std::vector<AccessGrantedCallback> m_accessGrantedCb; //!< Access granted CB
};

} // namespace ns3

#endif /* NR_CH_ACCESS_MANAGER_H_ */
