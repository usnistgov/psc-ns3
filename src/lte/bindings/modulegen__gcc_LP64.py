from pybindgen import Module, FileCodeSink, param, retval, cppclass, typehandlers


import pybindgen.settings
import warnings

class ErrorHandler(pybindgen.settings.ErrorHandler):
    def handle_error(self, wrapper, exception, traceback_):
        warnings.warn("exception %r in wrapper %s" % (exception, wrapper))
        return True
pybindgen.settings.error_handler = ErrorHandler()


import sys

def module_init():
    root_module = Module('ns.lte', cpp_namespace='::ns3')
    return root_module

def register_types(module):
    root_module = module.get_root()
    
    ## ff-mac-common.h (module 'lte'): ns3::Result_e [enumeration]
    module.add_enum('Result_e', ['SUCCESS', 'FAILURE'])
    ## ff-mac-common.h (module 'lte'): ns3::SetupRelease_e [enumeration]
    module.add_enum('SetupRelease_e', ['setup', 'release'])
    ## ff-mac-common.h (module 'lte'): ns3::CeBitmap_e [enumeration]
    module.add_enum('CeBitmap_e', ['TA', 'DRX', 'CR'])
    ## ff-mac-common.h (module 'lte'): ns3::NormalExtended_e [enumeration]
    module.add_enum('NormalExtended_e', ['normal', 'extended'])
    ## log.h (module 'core'): ns3::LogLevel [enumeration]
    module.add_enum('LogLevel', ['LOG_NONE', 'LOG_ERROR', 'LOG_LEVEL_ERROR', 'LOG_WARN', 'LOG_LEVEL_WARN', 'LOG_DEBUG', 'LOG_LEVEL_DEBUG', 'LOG_INFO', 'LOG_LEVEL_INFO', 'LOG_FUNCTION', 'LOG_LEVEL_FUNCTION', 'LOG_LOGIC', 'LOG_LEVEL_LOGIC', 'LOG_ALL', 'LOG_LEVEL_ALL', 'LOG_PREFIX_FUNC', 'LOG_PREFIX_TIME', 'LOG_PREFIX_NODE', 'LOG_PREFIX_LEVEL', 'LOG_PREFIX_ALL'], import_from_module='ns.core')
    ## address.h (module 'network'): ns3::Address [class]
    module.add_class('Address', import_from_module='ns.network')
    ## address.h (module 'network'): ns3::Address::MaxSize_e [enumeration]
    module.add_enum('MaxSize_e', ['MAX_SIZE'], outer_class=root_module['ns3::Address'], import_from_module='ns.network')
    ## eps-bearer.h (module 'lte'): ns3::AllocationRetentionPriority [struct]
    module.add_class('AllocationRetentionPriority')
    ## attribute-construction-list.h (module 'core'): ns3::AttributeConstructionList [class]
    module.add_class('AttributeConstructionList', import_from_module='ns.core')
    ## attribute-construction-list.h (module 'core'): ns3::AttributeConstructionList::Item [struct]
    module.add_class('Item', import_from_module='ns.core', outer_class=root_module['ns3::AttributeConstructionList'])
    typehandlers.add_type_alias(u'std::list< ns3::AttributeConstructionList::Item > const_iterator', u'ns3::AttributeConstructionList::CIterator')
    typehandlers.add_type_alias(u'std::list< ns3::AttributeConstructionList::Item > const_iterator*', u'ns3::AttributeConstructionList::CIterator*')
    typehandlers.add_type_alias(u'std::list< ns3::AttributeConstructionList::Item > const_iterator&', u'ns3::AttributeConstructionList::CIterator&')
    ## spectrum-model.h (module 'spectrum'): ns3::BandInfo [struct]
    module.add_class('BandInfo', import_from_module='ns.spectrum')
    ## buffer.h (module 'network'): ns3::Buffer [class]
    module.add_class('Buffer', import_from_module='ns.network')
    ## buffer.h (module 'network'): ns3::Buffer::Iterator [class]
    module.add_class('Iterator', import_from_module='ns.network', outer_class=root_module['ns3::Buffer'])
    ## lte-common.h (module 'lte'): ns3::BufferSizeLevelBsr [class]
    module.add_class('BufferSizeLevelBsr')
    ## ff-mac-common.h (module 'lte'): ns3::BuildBroadcastListElement_s [struct]
    module.add_class('BuildBroadcastListElement_s')
    ## ff-mac-common.h (module 'lte'): ns3::BuildBroadcastListElement_s::Type_e [enumeration]
    module.add_enum('Type_e', ['BCCH', 'PCCH'], outer_class=root_module['ns3::BuildBroadcastListElement_s'])
    ## ff-mac-common.h (module 'lte'): ns3::BuildDataListElement_s [struct]
    module.add_class('BuildDataListElement_s')
    ## ff-mac-common.h (module 'lte'): ns3::BuildRarListElement_s [struct]
    module.add_class('BuildRarListElement_s')
    ## buildings-helper.h (module 'buildings'): ns3::BuildingsHelper [class]
    module.add_class('BuildingsHelper', import_from_module='ns.buildings')
    ## ff-mac-common.h (module 'lte'): ns3::BwPart_s [struct]
    module.add_class('BwPart_s')
    ## packet.h (module 'network'): ns3::ByteTagIterator [class]
    module.add_class('ByteTagIterator', import_from_module='ns.network')
    ## packet.h (module 'network'): ns3::ByteTagIterator::Item [class]
    module.add_class('Item', import_from_module='ns.network', outer_class=root_module['ns3::ByteTagIterator'])
    ## byte-tag-list.h (module 'network'): ns3::ByteTagList [class]
    module.add_class('ByteTagList', import_from_module='ns.network')
    ## byte-tag-list.h (module 'network'): ns3::ByteTagList::Iterator [class]
    module.add_class('Iterator', import_from_module='ns.network', outer_class=root_module['ns3::ByteTagList'])
    ## byte-tag-list.h (module 'network'): ns3::ByteTagList::Iterator::Item [struct]
    module.add_class('Item', import_from_module='ns.network', outer_class=root_module['ns3::ByteTagList::Iterator'])
    ## callback.h (module 'core'): ns3::CallbackBase [class]
    module.add_class('CallbackBase', import_from_module='ns.core')
    ## cqa-ff-mac-scheduler.h (module 'lte'): ns3::CqasFlowPerf_t [struct]
    module.add_class('CqasFlowPerf_t')
    ## ff-mac-common.h (module 'lte'): ns3::CqiConfig_s [struct]
    module.add_class('CqiConfig_s')
    ## ff-mac-common.h (module 'lte'): ns3::CqiListElement_s [struct]
    module.add_class('CqiListElement_s')
    ## ff-mac-common.h (module 'lte'): ns3::CqiListElement_s::CqiType_e [enumeration]
    module.add_enum('CqiType_e', ['P10', 'P11', 'P20', 'P21', 'A12', 'A22', 'A20', 'A30', 'A31'], outer_class=root_module['ns3::CqiListElement_s'])
    ## data-output-interface.h (module 'stats'): ns3::DataOutputCallback [class]
    module.add_class('DataOutputCallback', allow_subclassing=True, import_from_module='ns.stats')
    ## data-rate.h (module 'network'): ns3::DataRate [class]
    module.add_class('DataRate', import_from_module='ns.network')
    ## default-deleter.h (module 'core'): ns3::DefaultDeleter<ns3::AttributeAccessor> [struct]
    module.add_class('DefaultDeleter', import_from_module='ns.core', template_parameters=['ns3::AttributeAccessor'])
    ## default-deleter.h (module 'core'): ns3::DefaultDeleter<ns3::AttributeChecker> [struct]
    module.add_class('DefaultDeleter', import_from_module='ns.core', template_parameters=['ns3::AttributeChecker'])
    ## default-deleter.h (module 'core'): ns3::DefaultDeleter<ns3::AttributeValue> [struct]
    module.add_class('DefaultDeleter', import_from_module='ns.core', template_parameters=['ns3::AttributeValue'])
    ## default-deleter.h (module 'core'): ns3::DefaultDeleter<ns3::CallbackImplBase> [struct]
    module.add_class('DefaultDeleter', import_from_module='ns.core', template_parameters=['ns3::CallbackImplBase'])
    ## default-deleter.h (module 'core'): ns3::DefaultDeleter<ns3::EpcTft> [struct]
    module.add_class('DefaultDeleter', template_parameters=['ns3::EpcTft'])
    ## default-deleter.h (module 'core'): ns3::DefaultDeleter<ns3::EventImpl> [struct]
    module.add_class('DefaultDeleter', import_from_module='ns.core', template_parameters=['ns3::EventImpl'])
    ## default-deleter.h (module 'core'): ns3::DefaultDeleter<ns3::Hash::Implementation> [struct]
    module.add_class('DefaultDeleter', import_from_module='ns.core', template_parameters=['ns3::Hash::Implementation'])
    ## default-deleter.h (module 'core'): ns3::DefaultDeleter<ns3::LteChunkProcessor> [struct]
    module.add_class('DefaultDeleter', template_parameters=['ns3::LteChunkProcessor'])
    ## default-deleter.h (module 'core'): ns3::DefaultDeleter<ns3::LteControlMessage> [struct]
    module.add_class('DefaultDeleter', template_parameters=['ns3::LteControlMessage'])
    ## default-deleter.h (module 'core'): ns3::DefaultDeleter<ns3::LteHarqPhy> [struct]
    module.add_class('DefaultDeleter', template_parameters=['ns3::LteHarqPhy'])
    ## default-deleter.h (module 'core'): ns3::DefaultDeleter<ns3::LteSlChunkProcessor> [struct]
    module.add_class('DefaultDeleter', template_parameters=['ns3::LteSlChunkProcessor'])
    ## default-deleter.h (module 'core'): ns3::DefaultDeleter<ns3::LteSlHarqPhy> [struct]
    module.add_class('DefaultDeleter', template_parameters=['ns3::LteSlHarqPhy'])
    ## default-deleter.h (module 'core'): ns3::DefaultDeleter<ns3::LteSlTft> [struct]
    module.add_class('DefaultDeleter', template_parameters=['ns3::LteSlTft'])
    ## default-deleter.h (module 'core'): ns3::DefaultDeleter<ns3::NixVector> [struct]
    module.add_class('DefaultDeleter', import_from_module='ns.core', template_parameters=['ns3::NixVector'])
    ## default-deleter.h (module 'core'): ns3::DefaultDeleter<ns3::Packet> [struct]
    module.add_class('DefaultDeleter', import_from_module='ns.core', template_parameters=['ns3::Packet'])
    ## default-deleter.h (module 'core'): ns3::DefaultDeleter<ns3::SpectrumModel> [struct]
    module.add_class('DefaultDeleter', import_from_module='ns.core', template_parameters=['ns3::SpectrumModel'])
    ## default-deleter.h (module 'core'): ns3::DefaultDeleter<ns3::SpectrumSignalParameters> [struct]
    module.add_class('DefaultDeleter', import_from_module='ns.core', template_parameters=['ns3::SpectrumSignalParameters'])
    ## default-deleter.h (module 'core'): ns3::DefaultDeleter<ns3::SpectrumValue> [struct]
    module.add_class('DefaultDeleter', import_from_module='ns.core', template_parameters=['ns3::SpectrumValue'])
    ## default-deleter.h (module 'core'): ns3::DefaultDeleter<ns3::TraceSourceAccessor> [struct]
    module.add_class('DefaultDeleter', import_from_module='ns.core', template_parameters=['ns3::TraceSourceAccessor'])
    ## default-deleter.h (module 'core'): ns3::DefaultDeleter<ns3::VendorSpecificValue> [struct]
    module.add_class('DefaultDeleter', template_parameters=['ns3::VendorSpecificValue'])
    ## default-deleter.h (module 'core'): ns3::DefaultDeleter<ns3::X2CellInfo> [struct]
    module.add_class('DefaultDeleter', template_parameters=['ns3::X2CellInfo'])
    ## default-deleter.h (module 'core'): ns3::DefaultDeleter<ns3::X2IfaceInfo> [struct]
    module.add_class('DefaultDeleter', template_parameters=['ns3::X2IfaceInfo'])
    ## ff-mac-common.h (module 'lte'): ns3::DlDciListElement_s [struct]
    module.add_class('DlDciListElement_s')
    ## ff-mac-common.h (module 'lte'): ns3::DlDciListElement_s::Format_e [enumeration]
    module.add_enum('Format_e', ['ONE', 'ONE_A', 'ONE_B', 'ONE_C', 'ONE_D', 'TWO', 'TWO_A', 'TWO_B'], outer_class=root_module['ns3::DlDciListElement_s'])
    ## ff-mac-common.h (module 'lte'): ns3::DlDciListElement_s::VrbFormat_e [enumeration]
    module.add_enum('VrbFormat_e', ['VRB_DISTRIBUTED', 'VRB_LOCALIZED'], outer_class=root_module['ns3::DlDciListElement_s'])
    ## ff-mac-common.h (module 'lte'): ns3::DlDciListElement_s::Ngap_e [enumeration]
    module.add_enum('Ngap_e', ['GAP1', 'GAP2'], outer_class=root_module['ns3::DlDciListElement_s'])
    ## ff-mac-common.h (module 'lte'): ns3::DlInfoListElement_s [struct]
    module.add_class('DlInfoListElement_s')
    ## ff-mac-common.h (module 'lte'): ns3::DlInfoListElement_s::HarqStatus_e [enumeration]
    module.add_enum('HarqStatus_e', ['ACK', 'NACK', 'DTX'], outer_class=root_module['ns3::DlInfoListElement_s'])
    ## lte-common.h (module 'lte'): ns3::DlSchedulingCallbackInfo [struct]
    module.add_class('DlSchedulingCallbackInfo')
    ## ff-mac-common.h (module 'lte'): ns3::DrxConfig_s [struct]
    module.add_class('DrxConfig_s')
    ## epc-enb-s1-sap.h (module 'lte'): ns3::EpcEnbS1SapProvider [class]
    module.add_class('EpcEnbS1SapProvider', allow_subclassing=True)
    ## epc-enb-s1-sap.h (module 'lte'): ns3::EpcEnbS1SapProvider::BearerToBeSwitched [struct]
    module.add_class('BearerToBeSwitched', outer_class=root_module['ns3::EpcEnbS1SapProvider'])
    ## epc-enb-s1-sap.h (module 'lte'): ns3::EpcEnbS1SapProvider::PathSwitchRequestParameters [struct]
    module.add_class('PathSwitchRequestParameters', outer_class=root_module['ns3::EpcEnbS1SapProvider'])
    ## epc-enb-s1-sap.h (module 'lte'): ns3::EpcEnbS1SapUser [class]
    module.add_class('EpcEnbS1SapUser', allow_subclassing=True)
    ## epc-enb-s1-sap.h (module 'lte'): ns3::EpcEnbS1SapUser::DataRadioBearerSetupRequestParameters [struct]
    module.add_class('DataRadioBearerSetupRequestParameters', outer_class=root_module['ns3::EpcEnbS1SapUser'])
    ## epc-enb-s1-sap.h (module 'lte'): ns3::EpcEnbS1SapUser::InitialContextSetupRequestParameters [struct]
    module.add_class('InitialContextSetupRequestParameters', outer_class=root_module['ns3::EpcEnbS1SapUser'])
    ## epc-enb-s1-sap.h (module 'lte'): ns3::EpcEnbS1SapUser::PathSwitchRequestAcknowledgeParameters [struct]
    module.add_class('PathSwitchRequestAcknowledgeParameters', outer_class=root_module['ns3::EpcEnbS1SapUser'])
    ## epc-s11-sap.h (module 'lte'): ns3::EpcS11Sap [class]
    module.add_class('EpcS11Sap')
    ## epc-s11-sap.h (module 'lte'): ns3::EpcS11Sap::Fteid [struct]
    module.add_class('Fteid', outer_class=root_module['ns3::EpcS11Sap'])
    ## epc-s11-sap.h (module 'lte'): ns3::EpcS11Sap::GtpcMessage [struct]
    module.add_class('GtpcMessage', outer_class=root_module['ns3::EpcS11Sap'])
    ## epc-s11-sap.h (module 'lte'): ns3::EpcS11Sap::Uli [struct]
    module.add_class('Uli', outer_class=root_module['ns3::EpcS11Sap'])
    ## epc-s11-sap.h (module 'lte'): ns3::EpcS11SapMme [class]
    module.add_class('EpcS11SapMme', parent=root_module['ns3::EpcS11Sap'])
    ## epc-s11-sap.h (module 'lte'): ns3::EpcS11SapMme::BearerContextCreated [struct]
    module.add_class('BearerContextCreated', outer_class=root_module['ns3::EpcS11SapMme'])
    ## epc-s11-sap.h (module 'lte'): ns3::EpcS11SapMme::BearerContextRemoved [struct]
    module.add_class('BearerContextRemoved', outer_class=root_module['ns3::EpcS11SapMme'])
    ## epc-s11-sap.h (module 'lte'): ns3::EpcS11SapMme::CreateSessionResponseMessage [struct]
    module.add_class('CreateSessionResponseMessage', parent=root_module['ns3::EpcS11Sap::GtpcMessage'], outer_class=root_module['ns3::EpcS11SapMme'])
    ## epc-s11-sap.h (module 'lte'): ns3::EpcS11SapMme::DeleteBearerRequestMessage [struct]
    module.add_class('DeleteBearerRequestMessage', parent=root_module['ns3::EpcS11Sap::GtpcMessage'], outer_class=root_module['ns3::EpcS11SapMme'])
    ## epc-s11-sap.h (module 'lte'): ns3::EpcS11SapMme::ModifyBearerResponseMessage [struct]
    module.add_class('ModifyBearerResponseMessage', parent=root_module['ns3::EpcS11Sap::GtpcMessage'], outer_class=root_module['ns3::EpcS11SapMme'])
    ## epc-s11-sap.h (module 'lte'): ns3::EpcS11SapMme::ModifyBearerResponseMessage::Cause [enumeration]
    module.add_enum('Cause', ['REQUEST_ACCEPTED', 'REQUEST_ACCEPTED_PARTIALLY', 'REQUEST_REJECTED', 'CONTEXT_NOT_FOUND'], outer_class=root_module['ns3::EpcS11SapMme::ModifyBearerResponseMessage'])
    ## epc-s11-sap.h (module 'lte'): ns3::EpcS11SapSgw [class]
    module.add_class('EpcS11SapSgw', parent=root_module['ns3::EpcS11Sap'])
    ## epc-s11-sap.h (module 'lte'): ns3::EpcS11SapSgw::BearerContextRemovedSgwPgw [struct]
    module.add_class('BearerContextRemovedSgwPgw', outer_class=root_module['ns3::EpcS11SapSgw'])
    ## epc-s11-sap.h (module 'lte'): ns3::EpcS11SapSgw::BearerContextToBeCreated [struct]
    module.add_class('BearerContextToBeCreated', outer_class=root_module['ns3::EpcS11SapSgw'])
    ## epc-s11-sap.h (module 'lte'): ns3::EpcS11SapSgw::BearerContextToBeRemoved [struct]
    module.add_class('BearerContextToBeRemoved', outer_class=root_module['ns3::EpcS11SapSgw'])
    ## epc-s11-sap.h (module 'lte'): ns3::EpcS11SapSgw::CreateSessionRequestMessage [struct]
    module.add_class('CreateSessionRequestMessage', parent=root_module['ns3::EpcS11Sap::GtpcMessage'], outer_class=root_module['ns3::EpcS11SapSgw'])
    ## epc-s11-sap.h (module 'lte'): ns3::EpcS11SapSgw::DeleteBearerCommandMessage [struct]
    module.add_class('DeleteBearerCommandMessage', parent=root_module['ns3::EpcS11Sap::GtpcMessage'], outer_class=root_module['ns3::EpcS11SapSgw'])
    ## epc-s11-sap.h (module 'lte'): ns3::EpcS11SapSgw::DeleteBearerResponseMessage [struct]
    module.add_class('DeleteBearerResponseMessage', parent=root_module['ns3::EpcS11Sap::GtpcMessage'], outer_class=root_module['ns3::EpcS11SapSgw'])
    ## epc-s11-sap.h (module 'lte'): ns3::EpcS11SapSgw::ModifyBearerRequestMessage [struct]
    module.add_class('ModifyBearerRequestMessage', parent=root_module['ns3::EpcS11Sap::GtpcMessage'], outer_class=root_module['ns3::EpcS11SapSgw'])
    ## epc-s1ap-sap.h (module 'lte'): ns3::EpcS1apSap [class]
    module.add_class('EpcS1apSap')
    ## epc-s1ap-sap.h (module 'lte'): ns3::EpcS1apSapEnb [class]
    module.add_class('EpcS1apSapEnb', parent=root_module['ns3::EpcS1apSap'])
    ## epc-s1ap-sap.h (module 'lte'): ns3::EpcS1apSapEnb::ErabSwitchedInUplinkItem [struct]
    module.add_class('ErabSwitchedInUplinkItem', outer_class=root_module['ns3::EpcS1apSapEnb'])
    ## epc-s1ap-sap.h (module 'lte'): ns3::EpcS1apSapEnb::ErabToBeSetupItem [struct]
    module.add_class('ErabToBeSetupItem', outer_class=root_module['ns3::EpcS1apSapEnb'])
    ## epc-s1ap-sap.h (module 'lte'): ns3::EpcS1apSapMme [class]
    module.add_class('EpcS1apSapMme', parent=root_module['ns3::EpcS1apSap'])
    ## epc-s1ap-sap.h (module 'lte'): ns3::EpcS1apSapMme::ErabSetupItem [struct]
    module.add_class('ErabSetupItem', outer_class=root_module['ns3::EpcS1apSapMme'])
    ## epc-s1ap-sap.h (module 'lte'): ns3::EpcS1apSapMme::ErabSwitchedInDownlinkItem [struct]
    module.add_class('ErabSwitchedInDownlinkItem', outer_class=root_module['ns3::EpcS1apSapMme'])
    ## epc-s1ap-sap.h (module 'lte'): ns3::EpcS1apSapMme::ErabToBeReleasedIndication [struct]
    module.add_class('ErabToBeReleasedIndication', outer_class=root_module['ns3::EpcS1apSapMme'])
    ## epc-x2-sap.h (module 'lte'): ns3::EpcX2Sap [class]
    module.add_class('EpcX2Sap')
    ## epc-x2-sap.h (module 'lte'): ns3::EpcX2Sap::UlInterferenceOverloadIndicationItem [enumeration]
    module.add_enum('UlInterferenceOverloadIndicationItem', ['HighInterference', 'MediumInterference', 'LowInterference'], outer_class=root_module['ns3::EpcX2Sap'])
    ## epc-x2-sap.h (module 'lte'): ns3::EpcX2Sap::LoadIndicator [enumeration]
    module.add_enum('LoadIndicator', ['LowLoad', 'MediumLoad', 'HighLoad', 'Overload'], outer_class=root_module['ns3::EpcX2Sap'])
    ## epc-x2-sap.h (module 'lte'): ns3::EpcX2Sap::IdCause [enumeration]
    module.add_enum('IdCause', ['HandoverDesirableForRadioReason', 'TimeCriticalHandover'], outer_class=root_module['ns3::EpcX2Sap'])
    ## epc-x2-sap.h (module 'lte'): ns3::EpcX2Sap::CellInformationItem [struct]
    module.add_class('CellInformationItem', outer_class=root_module['ns3::EpcX2Sap'])
    ## epc-x2-sap.h (module 'lte'): ns3::EpcX2Sap::CellMeasurementResultItem [struct]
    module.add_class('CellMeasurementResultItem', outer_class=root_module['ns3::EpcX2Sap'])
    ## epc-x2-sap.h (module 'lte'): ns3::EpcX2Sap::CompositeAvailCapacity [struct]
    module.add_class('CompositeAvailCapacity', outer_class=root_module['ns3::EpcX2Sap'])
    ## epc-x2-sap.h (module 'lte'): ns3::EpcX2Sap::ErabAdmittedItem [struct]
    module.add_class('ErabAdmittedItem', outer_class=root_module['ns3::EpcX2Sap'])
    ## epc-x2-sap.h (module 'lte'): ns3::EpcX2Sap::ErabNotAdmittedItem [struct]
    module.add_class('ErabNotAdmittedItem', outer_class=root_module['ns3::EpcX2Sap'])
    ## epc-x2-sap.h (module 'lte'): ns3::EpcX2Sap::ErabToBeSetupItem [struct]
    module.add_class('ErabToBeSetupItem', outer_class=root_module['ns3::EpcX2Sap'])
    ## epc-x2-sap.h (module 'lte'): ns3::EpcX2Sap::ErabsSubjectToStatusTransferItem [struct]
    module.add_class('ErabsSubjectToStatusTransferItem', outer_class=root_module['ns3::EpcX2Sap'])
    ## epc-x2-sap.h (module 'lte'): ns3::EpcX2Sap::HandoverPreparationFailureParams [struct]
    module.add_class('HandoverPreparationFailureParams', outer_class=root_module['ns3::EpcX2Sap'])
    ## epc-x2-sap.h (module 'lte'): ns3::EpcX2Sap::HandoverRequestAckParams [struct]
    module.add_class('HandoverRequestAckParams', outer_class=root_module['ns3::EpcX2Sap'])
    ## epc-x2-sap.h (module 'lte'): ns3::EpcX2Sap::HandoverRequestParams [struct]
    module.add_class('HandoverRequestParams', outer_class=root_module['ns3::EpcX2Sap'])
    ## epc-x2-sap.h (module 'lte'): ns3::EpcX2Sap::LoadInformationParams [struct]
    module.add_class('LoadInformationParams', outer_class=root_module['ns3::EpcX2Sap'])
    ## epc-x2-sap.h (module 'lte'): ns3::EpcX2Sap::RelativeNarrowbandTxBand [struct]
    module.add_class('RelativeNarrowbandTxBand', outer_class=root_module['ns3::EpcX2Sap'])
    ## epc-x2-sap.h (module 'lte'): ns3::EpcX2Sap::ResourceStatusUpdateParams [struct]
    module.add_class('ResourceStatusUpdateParams', outer_class=root_module['ns3::EpcX2Sap'])
    ## epc-x2-sap.h (module 'lte'): ns3::EpcX2Sap::SnStatusTransferParams [struct]
    module.add_class('SnStatusTransferParams', outer_class=root_module['ns3::EpcX2Sap'])
    ## epc-x2-sap.h (module 'lte'): ns3::EpcX2Sap::UeContextReleaseParams [struct]
    module.add_class('UeContextReleaseParams', outer_class=root_module['ns3::EpcX2Sap'])
    ## epc-x2-sap.h (module 'lte'): ns3::EpcX2Sap::UeDataParams [struct]
    module.add_class('UeDataParams', outer_class=root_module['ns3::EpcX2Sap'])
    ## epc-x2-sap.h (module 'lte'): ns3::EpcX2Sap::UlHighInterferenceInformationItem [struct]
    module.add_class('UlHighInterferenceInformationItem', outer_class=root_module['ns3::EpcX2Sap'])
    ## epc-x2-sap.h (module 'lte'): ns3::EpcX2SapProvider [class]
    module.add_class('EpcX2SapProvider', parent=root_module['ns3::EpcX2Sap'])
    ## epc-x2-sap.h (module 'lte'): ns3::EpcX2SapUser [class]
    module.add_class('EpcX2SapUser', parent=root_module['ns3::EpcX2Sap'])
    ## lte-common.h (module 'lte'): ns3::EutranMeasurementMapping [class]
    module.add_class('EutranMeasurementMapping')
    ## event-id.h (module 'core'): ns3::EventId [class]
    module.add_class('EventId', import_from_module='ns.core')
    ## ff-mac-csched-sap.h (module 'lte'): ns3::FfMacCschedSapProvider [class]
    module.add_class('FfMacCschedSapProvider', allow_subclassing=True)
    ## ff-mac-csched-sap.h (module 'lte'): ns3::FfMacCschedSapProvider::CschedCellConfigReqParameters [struct]
    module.add_class('CschedCellConfigReqParameters', outer_class=root_module['ns3::FfMacCschedSapProvider'])
    ## ff-mac-csched-sap.h (module 'lte'): ns3::FfMacCschedSapProvider::CschedCellConfigReqParameters::HoppingMode_e [enumeration]
    module.add_enum('HoppingMode_e', ['inter', 'interintra'], outer_class=root_module['ns3::FfMacCschedSapProvider::CschedCellConfigReqParameters'])
    ## ff-mac-csched-sap.h (module 'lte'): ns3::FfMacCschedSapProvider::CschedCellConfigReqParameters::PhichResource_e [enumeration]
    module.add_enum('PhichResource_e', ['PHICH_R_ONE_SIXTH', 'PHICH_R_HALF', 'PHICH_R_ONE', 'PHICH_R_TWO'], outer_class=root_module['ns3::FfMacCschedSapProvider::CschedCellConfigReqParameters'])
    ## ff-mac-csched-sap.h (module 'lte'): ns3::FfMacCschedSapProvider::CschedCellConfigReqParameters::DuplexMode_e [enumeration]
    module.add_enum('DuplexMode_e', ['DM_TDD', 'DM_FDD'], outer_class=root_module['ns3::FfMacCschedSapProvider::CschedCellConfigReqParameters'])
    ## ff-mac-csched-sap.h (module 'lte'): ns3::FfMacCschedSapProvider::CschedCellConfigReqParameters::Enable64Qam_e [enumeration]
    module.add_enum('Enable64Qam_e', ['MOD_16QAM', 'MOD_64QAM'], outer_class=root_module['ns3::FfMacCschedSapProvider::CschedCellConfigReqParameters'])
    ## ff-mac-csched-sap.h (module 'lte'): ns3::FfMacCschedSapProvider::CschedDiscPoolConfigReqParameters [struct]
    module.add_class('CschedDiscPoolConfigReqParameters', outer_class=root_module['ns3::FfMacCschedSapProvider'])
    ## ff-mac-csched-sap.h (module 'lte'): ns3::FfMacCschedSapProvider::CschedDiscPoolReleaseReqParameters [struct]
    module.add_class('CschedDiscPoolReleaseReqParameters', outer_class=root_module['ns3::FfMacCschedSapProvider'])
    ## ff-mac-csched-sap.h (module 'lte'): ns3::FfMacCschedSapProvider::CschedLcConfigReqParameters [struct]
    module.add_class('CschedLcConfigReqParameters', outer_class=root_module['ns3::FfMacCschedSapProvider'])
    ## ff-mac-csched-sap.h (module 'lte'): ns3::FfMacCschedSapProvider::CschedLcReleaseReqParameters [struct]
    module.add_class('CschedLcReleaseReqParameters', outer_class=root_module['ns3::FfMacCschedSapProvider'])
    ## ff-mac-csched-sap.h (module 'lte'): ns3::FfMacCschedSapProvider::CschedPoolConfigReqParameters [struct]
    module.add_class('CschedPoolConfigReqParameters', outer_class=root_module['ns3::FfMacCschedSapProvider'])
    ## ff-mac-csched-sap.h (module 'lte'): ns3::FfMacCschedSapProvider::CschedPoolReleaseReqParameters [struct]
    module.add_class('CschedPoolReleaseReqParameters', outer_class=root_module['ns3::FfMacCschedSapProvider'])
    ## ff-mac-csched-sap.h (module 'lte'): ns3::FfMacCschedSapProvider::CschedUeConfigReqParameters [struct]
    module.add_class('CschedUeConfigReqParameters', outer_class=root_module['ns3::FfMacCschedSapProvider'])
    ## ff-mac-csched-sap.h (module 'lte'): ns3::FfMacCschedSapProvider::CschedUeConfigReqParameters::MeasGapConfigPattern_e [enumeration]
    module.add_enum('MeasGapConfigPattern_e', ['MGP_GP1', 'MGP_GP2', 'OFF'], outer_class=root_module['ns3::FfMacCschedSapProvider::CschedUeConfigReqParameters'])
    ## ff-mac-csched-sap.h (module 'lte'): ns3::FfMacCschedSapProvider::CschedUeConfigReqParameters::OpenClosedLoop_e [enumeration]
    module.add_enum('OpenClosedLoop_e', ['noneloop', 'openloop', 'closedloop'], outer_class=root_module['ns3::FfMacCschedSapProvider::CschedUeConfigReqParameters'])
    ## ff-mac-csched-sap.h (module 'lte'): ns3::FfMacCschedSapProvider::CschedUeConfigReqParameters::RepMode_e [enumeration]
    module.add_enum('RepMode_e', ['rm12', 'rm20', 'rm22', 'rm30', 'rm31', 'nonemode'], outer_class=root_module['ns3::FfMacCschedSapProvider::CschedUeConfigReqParameters'])
    ## ff-mac-csched-sap.h (module 'lte'): ns3::FfMacCschedSapProvider::CschedUeConfigReqParameters::FeedbackMode_e [enumeration]
    module.add_enum('FeedbackMode_e', ['bundling', 'multiplexing'], outer_class=root_module['ns3::FfMacCschedSapProvider::CschedUeConfigReqParameters'])
    ## ff-mac-csched-sap.h (module 'lte'): ns3::FfMacCschedSapProvider::CschedUeReleaseReqParameters [struct]
    module.add_class('CschedUeReleaseReqParameters', outer_class=root_module['ns3::FfMacCschedSapProvider'])
    ## ff-mac-csched-sap.h (module 'lte'): ns3::FfMacCschedSapUser [class]
    module.add_class('FfMacCschedSapUser', allow_subclassing=True)
    ## ff-mac-csched-sap.h (module 'lte'): ns3::FfMacCschedSapUser::CschedCellConfigCnfParameters [struct]
    module.add_class('CschedCellConfigCnfParameters', outer_class=root_module['ns3::FfMacCschedSapUser'])
    ## ff-mac-csched-sap.h (module 'lte'): ns3::FfMacCschedSapUser::CschedCellConfigUpdateIndParameters [struct]
    module.add_class('CschedCellConfigUpdateIndParameters', outer_class=root_module['ns3::FfMacCschedSapUser'])
    ## ff-mac-csched-sap.h (module 'lte'): ns3::FfMacCschedSapUser::CschedLcConfigCnfParameters [struct]
    module.add_class('CschedLcConfigCnfParameters', outer_class=root_module['ns3::FfMacCschedSapUser'])
    ## ff-mac-csched-sap.h (module 'lte'): ns3::FfMacCschedSapUser::CschedLcReleaseCnfParameters [struct]
    module.add_class('CschedLcReleaseCnfParameters', outer_class=root_module['ns3::FfMacCschedSapUser'])
    ## ff-mac-csched-sap.h (module 'lte'): ns3::FfMacCschedSapUser::CschedUeConfigCnfParameters [struct]
    module.add_class('CschedUeConfigCnfParameters', outer_class=root_module['ns3::FfMacCschedSapUser'])
    ## ff-mac-csched-sap.h (module 'lte'): ns3::FfMacCschedSapUser::CschedUeConfigUpdateIndParameters [struct]
    module.add_class('CschedUeConfigUpdateIndParameters', outer_class=root_module['ns3::FfMacCschedSapUser'])
    ## ff-mac-csched-sap.h (module 'lte'): ns3::FfMacCschedSapUser::CschedUeReleaseCnfParameters [struct]
    module.add_class('CschedUeReleaseCnfParameters', outer_class=root_module['ns3::FfMacCschedSapUser'])
    ## ff-mac-sched-sap.h (module 'lte'): ns3::FfMacSchedSapProvider [class]
    module.add_class('FfMacSchedSapProvider', allow_subclassing=True)
    ## ff-mac-sched-sap.h (module 'lte'): ns3::FfMacSchedSapProvider::SchedDlCqiInfoReqParameters [struct]
    module.add_class('SchedDlCqiInfoReqParameters', outer_class=root_module['ns3::FfMacSchedSapProvider'])
    ## ff-mac-sched-sap.h (module 'lte'): ns3::FfMacSchedSapProvider::SchedDlMacBufferReqParameters [struct]
    module.add_class('SchedDlMacBufferReqParameters', outer_class=root_module['ns3::FfMacSchedSapProvider'])
    ## ff-mac-sched-sap.h (module 'lte'): ns3::FfMacSchedSapProvider::SchedDlPagingBufferReqParameters [struct]
    module.add_class('SchedDlPagingBufferReqParameters', outer_class=root_module['ns3::FfMacSchedSapProvider'])
    ## ff-mac-sched-sap.h (module 'lte'): ns3::FfMacSchedSapProvider::SchedDlRachInfoReqParameters [struct]
    module.add_class('SchedDlRachInfoReqParameters', outer_class=root_module['ns3::FfMacSchedSapProvider'])
    ## ff-mac-sched-sap.h (module 'lte'): ns3::FfMacSchedSapProvider::SchedDlRlcBufferReqParameters [struct]
    module.add_class('SchedDlRlcBufferReqParameters', outer_class=root_module['ns3::FfMacSchedSapProvider'])
    ## ff-mac-sched-sap.h (module 'lte'): ns3::FfMacSchedSapProvider::SchedDlTriggerReqParameters [struct]
    module.add_class('SchedDlTriggerReqParameters', outer_class=root_module['ns3::FfMacSchedSapProvider'])
    ## ff-mac-sched-sap.h (module 'lte'): ns3::FfMacSchedSapProvider::SchedUlCqiInfoReqParameters [struct]
    module.add_class('SchedUlCqiInfoReqParameters', outer_class=root_module['ns3::FfMacSchedSapProvider'])
    ## ff-mac-sched-sap.h (module 'lte'): ns3::FfMacSchedSapProvider::SchedUlMacCtrlInfoReqParameters [struct]
    module.add_class('SchedUlMacCtrlInfoReqParameters', outer_class=root_module['ns3::FfMacSchedSapProvider'])
    ## ff-mac-sched-sap.h (module 'lte'): ns3::FfMacSchedSapProvider::SchedUlNoiseInterferenceReqParameters [struct]
    module.add_class('SchedUlNoiseInterferenceReqParameters', outer_class=root_module['ns3::FfMacSchedSapProvider'])
    ## ff-mac-sched-sap.h (module 'lte'): ns3::FfMacSchedSapProvider::SchedUlSrInfoReqParameters [struct]
    module.add_class('SchedUlSrInfoReqParameters', outer_class=root_module['ns3::FfMacSchedSapProvider'])
    ## ff-mac-sched-sap.h (module 'lte'): ns3::FfMacSchedSapProvider::SchedUlTriggerReqParameters [struct]
    module.add_class('SchedUlTriggerReqParameters', outer_class=root_module['ns3::FfMacSchedSapProvider'])
    ## ff-mac-sched-sap.h (module 'lte'): ns3::FfMacSchedSapUser [class]
    module.add_class('FfMacSchedSapUser', allow_subclassing=True)
    ## ff-mac-sched-sap.h (module 'lte'): ns3::FfMacSchedSapUser::SchedDlConfigIndParameters [struct]
    module.add_class('SchedDlConfigIndParameters', outer_class=root_module['ns3::FfMacSchedSapUser'])
    ## ff-mac-sched-sap.h (module 'lte'): ns3::FfMacSchedSapUser::SchedUlConfigIndParameters [struct]
    module.add_class('SchedUlConfigIndParameters', outer_class=root_module['ns3::FfMacSchedSapUser'])
    ## eps-bearer.h (module 'lte'): ns3::GbrQosInformation [struct]
    module.add_class('GbrQosInformation')
    ## epc-gtpc-header.h (module 'lte'): ns3::GtpcIes [class]
    module.add_class('GtpcIes')
    ## epc-gtpc-header.h (module 'lte'): ns3::GtpcIes::Cause_t [enumeration]
    module.add_enum('Cause_t', ['RESERVED', 'REQUEST_ACCEPTED'], outer_class=root_module['ns3::GtpcIes'])
    ## lte-harq-phy.h (module 'lte'): ns3::HarqProcessInfoElement_t [struct]
    module.add_class('HarqProcessInfoElement_t')
    ## hash.h (module 'core'): ns3::Hasher [class]
    module.add_class('Hasher', import_from_module='ns.core')
    ## ff-mac-common.h (module 'lte'): ns3::HigherLayerSelected_s [struct]
    module.add_class('HigherLayerSelected_s')
    ## lte-common.h (module 'lte'): ns3::ImsiLcidPair_t [struct]
    module.add_class('ImsiLcidPair_t')
    ## inet6-socket-address.h (module 'network'): ns3::Inet6SocketAddress [class]
    module.add_class('Inet6SocketAddress', import_from_module='ns.network')
    ## inet6-socket-address.h (module 'network'): ns3::Inet6SocketAddress [class]
    root_module['ns3::Inet6SocketAddress'].implicitly_converts_to(root_module['ns3::Address'])
    ## inet-socket-address.h (module 'network'): ns3::InetSocketAddress [class]
    module.add_class('InetSocketAddress', import_from_module='ns.network')
    ## inet-socket-address.h (module 'network'): ns3::InetSocketAddress [class]
    root_module['ns3::InetSocketAddress'].implicitly_converts_to(root_module['ns3::Address'])
    ## ipv4-address.h (module 'network'): ns3::Ipv4Address [class]
    module.add_class('Ipv4Address', import_from_module='ns.network')
    ## ipv4-address.h (module 'network'): ns3::Ipv4Address [class]
    root_module['ns3::Ipv4Address'].implicitly_converts_to(root_module['ns3::Address'])
    ## ipv4-address-helper.h (module 'internet'): ns3::Ipv4AddressHelper [class]
    module.add_class('Ipv4AddressHelper', import_from_module='ns.internet')
    ## ipv4-interface-address.h (module 'internet'): ns3::Ipv4InterfaceAddress [class]
    module.add_class('Ipv4InterfaceAddress', import_from_module='ns.internet')
    ## ipv4-interface-address.h (module 'internet'): ns3::Ipv4InterfaceAddress::InterfaceAddressScope_e [enumeration]
    module.add_enum('InterfaceAddressScope_e', ['HOST', 'LINK', 'GLOBAL'], outer_class=root_module['ns3::Ipv4InterfaceAddress'], import_from_module='ns.internet')
    ## ipv4-interface-container.h (module 'internet'): ns3::Ipv4InterfaceContainer [class]
    module.add_class('Ipv4InterfaceContainer', import_from_module='ns.internet')
    typehandlers.add_type_alias(u'std::vector< std::pair< ns3::Ptr< ns3::Ipv4 >, unsigned int > > const_iterator', u'ns3::Ipv4InterfaceContainer::Iterator')
    typehandlers.add_type_alias(u'std::vector< std::pair< ns3::Ptr< ns3::Ipv4 >, unsigned int > > const_iterator*', u'ns3::Ipv4InterfaceContainer::Iterator*')
    typehandlers.add_type_alias(u'std::vector< std::pair< ns3::Ptr< ns3::Ipv4 >, unsigned int > > const_iterator&', u'ns3::Ipv4InterfaceContainer::Iterator&')
    ## ipv4-address.h (module 'network'): ns3::Ipv4Mask [class]
    module.add_class('Ipv4Mask', import_from_module='ns.network')
    ## ipv6-address.h (module 'network'): ns3::Ipv6Address [class]
    module.add_class('Ipv6Address', import_from_module='ns.network')
    ## ipv6-address.h (module 'network'): ns3::Ipv6Address [class]
    root_module['ns3::Ipv6Address'].implicitly_converts_to(root_module['ns3::Address'])
    ## ipv6-address-helper.h (module 'internet'): ns3::Ipv6AddressHelper [class]
    module.add_class('Ipv6AddressHelper', import_from_module='ns.internet')
    ## ipv6-interface-address.h (module 'internet'): ns3::Ipv6InterfaceAddress [class]
    module.add_class('Ipv6InterfaceAddress', import_from_module='ns.internet')
    ## ipv6-interface-address.h (module 'internet'): ns3::Ipv6InterfaceAddress::State_e [enumeration]
    module.add_enum('State_e', ['TENTATIVE', 'DEPRECATED', 'PREFERRED', 'PERMANENT', 'HOMEADDRESS', 'TENTATIVE_OPTIMISTIC', 'INVALID'], outer_class=root_module['ns3::Ipv6InterfaceAddress'], import_from_module='ns.internet')
    ## ipv6-interface-address.h (module 'internet'): ns3::Ipv6InterfaceAddress::Scope_e [enumeration]
    module.add_enum('Scope_e', ['HOST', 'LINKLOCAL', 'GLOBAL'], outer_class=root_module['ns3::Ipv6InterfaceAddress'], import_from_module='ns.internet')
    ## ipv6-interface-container.h (module 'internet'): ns3::Ipv6InterfaceContainer [class]
    module.add_class('Ipv6InterfaceContainer', import_from_module='ns.internet')
    typehandlers.add_type_alias(u'std::vector< std::pair< ns3::Ptr< ns3::Ipv6 >, unsigned int > > const_iterator', u'ns3::Ipv6InterfaceContainer::Iterator')
    typehandlers.add_type_alias(u'std::vector< std::pair< ns3::Ptr< ns3::Ipv6 >, unsigned int > > const_iterator*', u'ns3::Ipv6InterfaceContainer::Iterator*')
    typehandlers.add_type_alias(u'std::vector< std::pair< ns3::Ptr< ns3::Ipv6 >, unsigned int > > const_iterator&', u'ns3::Ipv6InterfaceContainer::Iterator&')
    ## ipv6-address.h (module 'network'): ns3::Ipv6Prefix [class]
    module.add_class('Ipv6Prefix', import_from_module='ns.network')
    ## log.h (module 'core'): ns3::LogComponent [class]
    module.add_class('LogComponent', import_from_module='ns.core')
    typehandlers.add_type_alias(u'std::map< std::string, ns3::LogComponent * >', u'ns3::LogComponent::ComponentList')
    typehandlers.add_type_alias(u'std::map< std::string, ns3::LogComponent * >*', u'ns3::LogComponent::ComponentList*')
    typehandlers.add_type_alias(u'std::map< std::string, ns3::LogComponent * >&', u'ns3::LogComponent::ComponentList&')
    ## ff-mac-common.h (module 'lte'): ns3::LogicalChannelConfigListElement_s [struct]
    module.add_class('LogicalChannelConfigListElement_s')
    ## ff-mac-common.h (module 'lte'): ns3::LogicalChannelConfigListElement_s::Direction_e [enumeration]
    module.add_enum('Direction_e', ['DIR_UL', 'DIR_DL', 'DIR_BOTH'], outer_class=root_module['ns3::LogicalChannelConfigListElement_s'])
    ## ff-mac-common.h (module 'lte'): ns3::LogicalChannelConfigListElement_s::QosBearerType_e [enumeration]
    module.add_enum('QosBearerType_e', ['QBT_NON_GBR', 'QBT_GBR'], outer_class=root_module['ns3::LogicalChannelConfigListElement_s'])
    ## lte-anr-sap.h (module 'lte'): ns3::LteAnrSapProvider [class]
    module.add_class('LteAnrSapProvider', allow_subclassing=True)
    ## lte-anr-sap.h (module 'lte'): ns3::LteAnrSapUser [class]
    module.add_class('LteAnrSapUser', allow_subclassing=True)
    ## lte-as-sap.h (module 'lte'): ns3::LteAsSapProvider [class]
    module.add_class('LteAsSapProvider', allow_subclassing=True)
    ## lte-as-sap.h (module 'lte'): ns3::LteAsSapUser [class]
    module.add_class('LteAsSapUser', allow_subclassing=True)
    ## lte-ccm-mac-sap.h (module 'lte'): ns3::LteCcmMacSapProvider [class]
    module.add_class('LteCcmMacSapProvider', allow_subclassing=True)
    ## lte-ccm-rrc-sap.h (module 'lte'): ns3::LteCcmRrcSapProvider [class]
    module.add_class('LteCcmRrcSapProvider', allow_subclassing=True)
    ## lte-ccm-rrc-sap.h (module 'lte'): ns3::LteCcmRrcSapProvider::LcsConfig [struct]
    module.add_class('LcsConfig', outer_class=root_module['ns3::LteCcmRrcSapProvider'])
    ## lte-ccm-rrc-sap.h (module 'lte'): ns3::LteCcmRrcSapUser [class]
    module.add_class('LteCcmRrcSapUser', allow_subclassing=True)
    ## lte-enb-cmac-sap.h (module 'lte'): ns3::LteEnbCmacSapProvider [class]
    module.add_class('LteEnbCmacSapProvider', allow_subclassing=True)
    ## lte-enb-cmac-sap.h (module 'lte'): ns3::LteEnbCmacSapProvider::AllocateNcRaPreambleReturnValue [struct]
    module.add_class('AllocateNcRaPreambleReturnValue', outer_class=root_module['ns3::LteEnbCmacSapProvider'])
    ## lte-enb-cmac-sap.h (module 'lte'): ns3::LteEnbCmacSapProvider::LcInfo [struct]
    module.add_class('LcInfo', outer_class=root_module['ns3::LteEnbCmacSapProvider'])
    ## lte-enb-cmac-sap.h (module 'lte'): ns3::LteEnbCmacSapProvider::RachConfig [struct]
    module.add_class('RachConfig', outer_class=root_module['ns3::LteEnbCmacSapProvider'])
    ## lte-enb-cmac-sap.h (module 'lte'): ns3::LteEnbCmacSapProvider::UeConfig [struct]
    module.add_class('UeConfig', outer_class=root_module['ns3::LteEnbCmacSapProvider'])
    ## lte-enb-cmac-sap.h (module 'lte'): ns3::LteEnbCmacSapUser [class]
    module.add_class('LteEnbCmacSapUser', allow_subclassing=True)
    ## lte-enb-cmac-sap.h (module 'lte'): ns3::LteEnbCmacSapUser::UeConfig [struct]
    module.add_class('UeConfig', outer_class=root_module['ns3::LteEnbCmacSapUser'])
    ## lte-enb-cphy-sap.h (module 'lte'): ns3::LteEnbCphySapProvider [class]
    module.add_class('LteEnbCphySapProvider', allow_subclassing=True)
    ## lte-enb-cphy-sap.h (module 'lte'): ns3::LteEnbCphySapUser [class]
    module.add_class('LteEnbCphySapUser')
    ## lte-enb-phy-sap.h (module 'lte'): ns3::LteEnbPhySapProvider [class]
    module.add_class('LteEnbPhySapProvider', allow_subclassing=True)
    ## lte-enb-phy-sap.h (module 'lte'): ns3::LteEnbPhySapUser [class]
    module.add_class('LteEnbPhySapUser', allow_subclassing=True)
    ## lte-common.h (module 'lte'): ns3::LteFfConverter [class]
    module.add_class('LteFfConverter')
    ## lte-ffr-rrc-sap.h (module 'lte'): ns3::LteFfrRrcSapProvider [class]
    module.add_class('LteFfrRrcSapProvider', allow_subclassing=True)
    ## lte-ffr-rrc-sap.h (module 'lte'): ns3::LteFfrRrcSapUser [class]
    module.add_class('LteFfrRrcSapUser', allow_subclassing=True)
    ## lte-ffr-sap.h (module 'lte'): ns3::LteFfrSapProvider [class]
    module.add_class('LteFfrSapProvider', allow_subclassing=True)
    ## lte-ffr-sap.h (module 'lte'): ns3::LteFfrSapUser [class]
    module.add_class('LteFfrSapUser')
    ## lte-common.h (module 'lte'): ns3::LteFlowId_t [struct]
    module.add_class('LteFlowId_t')
    ## lte-global-pathloss-database.h (module 'lte'): ns3::LteGlobalPathlossDatabase [class]
    module.add_class('LteGlobalPathlossDatabase', allow_subclassing=True)
    ## lte-handover-management-sap.h (module 'lte'): ns3::LteHandoverManagementSapProvider [class]
    module.add_class('LteHandoverManagementSapProvider', allow_subclassing=True)
    ## lte-handover-management-sap.h (module 'lte'): ns3::LteHandoverManagementSapUser [class]
    module.add_class('LteHandoverManagementSapUser', allow_subclassing=True)
    ## lte-mac-sap.h (module 'lte'): ns3::LteMacSapProvider [class]
    module.add_class('LteMacSapProvider', allow_subclassing=True)
    ## lte-mac-sap.h (module 'lte'): ns3::LteMacSapProvider::ReportBufferStatusParameters [struct]
    module.add_class('ReportBufferStatusParameters', outer_class=root_module['ns3::LteMacSapProvider'])
    ## lte-mac-sap.h (module 'lte'): ns3::LteMacSapProvider::TransmitPduParameters [struct]
    module.add_class('TransmitPduParameters', outer_class=root_module['ns3::LteMacSapProvider'])
    ## lte-mac-sap.h (module 'lte'): ns3::LteMacSapUser [class]
    module.add_class('LteMacSapUser', allow_subclassing=True)
    ## lte-mac-sap.h (module 'lte'): ns3::LteMacSapUser::ReceivePduParameters [struct]
    module.add_class('ReceivePduParameters', outer_class=root_module['ns3::LteMacSapUser'])
    ## lte-mac-sap.h (module 'lte'): ns3::LteMacSapUser::TxOpportunityParameters [struct]
    module.add_class('TxOpportunityParameters', outer_class=root_module['ns3::LteMacSapUser'])
    ## lte-mi-error-model.h (module 'lte'): ns3::LteMiErrorModel [class]
    module.add_class('LteMiErrorModel')
    ## lte-nist-error-model.h (module 'lte'): ns3::LteNistErrorModel [class]
    module.add_class('LteNistErrorModel')
    ## lte-nist-error-model.h (module 'lte'): ns3::LteNistErrorModel::LtePhyChannel [enumeration]
    module.add_enum('LtePhyChannel', ['PDCCH', 'PDSCH', 'PUCCH', 'PUSCH', 'PSCCH', 'PSSCH', 'PSDCH'], outer_class=root_module['ns3::LteNistErrorModel'])
    ## lte-nist-error-model.h (module 'lte'): ns3::LteNistErrorModel::LteFadingModel [enumeration]
    module.add_enum('LteFadingModel', ['AWGN'], outer_class=root_module['ns3::LteNistErrorModel'])
    ## lte-nist-error-model.h (module 'lte'): ns3::LteNistErrorModel::LteTxMode [enumeration]
    module.add_enum('LteTxMode', ['SISO', 'SIMO', 'TxDiversity', 'SpatMultplex'], outer_class=root_module['ns3::LteNistErrorModel'])
    ## lte-pdcp-sap.h (module 'lte'): ns3::LtePdcpSapProvider [class]
    module.add_class('LtePdcpSapProvider', allow_subclassing=True)
    ## lte-pdcp-sap.h (module 'lte'): ns3::LtePdcpSapProvider::TransmitPdcpSduParameters [struct]
    module.add_class('TransmitPdcpSduParameters', outer_class=root_module['ns3::LtePdcpSapProvider'])
    ## lte-pdcp-sap.h (module 'lte'): ns3::LtePdcpSapUser [class]
    module.add_class('LtePdcpSapUser', allow_subclassing=True)
    ## lte-pdcp-sap.h (module 'lte'): ns3::LtePdcpSapUser::ReceivePdcpSduParameters [struct]
    module.add_class('ReceivePdcpSduParameters', outer_class=root_module['ns3::LtePdcpSapUser'])
    ## lte-rlc-sap.h (module 'lte'): ns3::LteRlcSapProvider [class]
    module.add_class('LteRlcSapProvider', allow_subclassing=True)
    ## lte-rlc-sap.h (module 'lte'): ns3::LteRlcSapProvider::TransmitPdcpPduParameters [struct]
    module.add_class('TransmitPdcpPduParameters', outer_class=root_module['ns3::LteRlcSapProvider'])
    ## lte-rlc-sap.h (module 'lte'): ns3::LteRlcSapUser [class]
    module.add_class('LteRlcSapUser', allow_subclassing=True)
    ## lte-rrc-sap.h (module 'lte'): ns3::LteRrcSap [class]
    module.add_class('LteRrcSap')
    ## lte-rrc-sap.h (module 'lte'): ns3::LteRrcSap::ReestablishmentCause [enumeration]
    module.add_enum('ReestablishmentCause', ['RECONFIGURATION_FAILURE', 'HANDOVER_FAILURE', 'OTHER_FAILURE'], outer_class=root_module['ns3::LteRrcSap'])
    ## lte-rrc-sap.h (module 'lte'): ns3::LteRrcSap::AntennaInfoCommon [struct]
    module.add_class('AntennaInfoCommon', outer_class=root_module['ns3::LteRrcSap'])
    ## lte-rrc-sap.h (module 'lte'): ns3::LteRrcSap::AntennaInfoDedicated [struct]
    module.add_class('AntennaInfoDedicated', outer_class=root_module['ns3::LteRrcSap'])
    ## lte-rrc-sap.h (module 'lte'): ns3::LteRrcSap::AntennaInfoUl [struct]
    module.add_class('AntennaInfoUl', outer_class=root_module['ns3::LteRrcSap'])
    ## lte-rrc-sap.h (module 'lte'): ns3::LteRrcSap::AsConfig [struct]
    module.add_class('AsConfig', outer_class=root_module['ns3::LteRrcSap'])
    ## lte-rrc-sap.h (module 'lte'): ns3::LteRrcSap::BlackCellsToAddMod [struct]
    module.add_class('BlackCellsToAddMod', outer_class=root_module['ns3::LteRrcSap'])
    ## lte-rrc-sap.h (module 'lte'): ns3::LteRrcSap::CarrierBandwidthEutra [struct]
    module.add_class('CarrierBandwidthEutra', outer_class=root_module['ns3::LteRrcSap'])
    ## lte-rrc-sap.h (module 'lte'): ns3::LteRrcSap::CarrierFreqEutra [struct]
    module.add_class('CarrierFreqEutra', outer_class=root_module['ns3::LteRrcSap'])
    ## lte-rrc-sap.h (module 'lte'): ns3::LteRrcSap::CellAccessRelatedInfo [struct]
    module.add_class('CellAccessRelatedInfo', outer_class=root_module['ns3::LteRrcSap'])
    ## lte-rrc-sap.h (module 'lte'): ns3::LteRrcSap::CellIdentification [struct]
    module.add_class('CellIdentification', outer_class=root_module['ns3::LteRrcSap'])
    ## lte-rrc-sap.h (module 'lte'): ns3::LteRrcSap::CellSelectionInfo [struct]
    module.add_class('CellSelectionInfo', outer_class=root_module['ns3::LteRrcSap'])
    ## lte-rrc-sap.h (module 'lte'): ns3::LteRrcSap::CellsToAddMod [struct]
    module.add_class('CellsToAddMod', outer_class=root_module['ns3::LteRrcSap'])
    ## lte-rrc-sap.h (module 'lte'): ns3::LteRrcSap::CgiInfo [struct]
    module.add_class('CgiInfo', outer_class=root_module['ns3::LteRrcSap'])
    ## lte-rrc-sap.h (module 'lte'): ns3::LteRrcSap::DrbToAddMod [struct]
    module.add_class('DrbToAddMod', outer_class=root_module['ns3::LteRrcSap'])
    ## lte-rrc-sap.h (module 'lte'): ns3::LteRrcSap::FreqInfo [struct]
    module.add_class('FreqInfo', outer_class=root_module['ns3::LteRrcSap'])
    ## lte-rrc-sap.h (module 'lte'): ns3::LteRrcSap::HandoverPreparationInfo [struct]
    module.add_class('HandoverPreparationInfo', outer_class=root_module['ns3::LteRrcSap'])
    ## lte-rrc-sap.h (module 'lte'): ns3::LteRrcSap::LogicalChannelConfig [struct]
    module.add_class('LogicalChannelConfig', outer_class=root_module['ns3::LteRrcSap'])
    ## lte-rrc-sap.h (module 'lte'): ns3::LteRrcSap::MasterInformationBlock [struct]
    module.add_class('MasterInformationBlock', outer_class=root_module['ns3::LteRrcSap'])
    ## lte-rrc-sap.h (module 'lte'): ns3::LteRrcSap::MasterInformationBlockSL [struct]
    module.add_class('MasterInformationBlockSL', outer_class=root_module['ns3::LteRrcSap'])
    ## lte-rrc-sap.h (module 'lte'): ns3::LteRrcSap::MeasConfig [struct]
    module.add_class('MeasConfig', outer_class=root_module['ns3::LteRrcSap'])
    ## lte-rrc-sap.h (module 'lte'): ns3::LteRrcSap::MeasGapConfig [struct]
    module.add_class('MeasGapConfig', outer_class=root_module['ns3::LteRrcSap'])
    ## lte-rrc-sap.h (module 'lte'): ns3::LteRrcSap::MeasGapConfig::action [enumeration]
    module.add_enum('action', ['SETUP', 'RESET'], outer_class=root_module['ns3::LteRrcSap::MeasGapConfig'])
    ## lte-rrc-sap.h (module 'lte'): ns3::LteRrcSap::MeasGapConfig::gap [enumeration]
    module.add_enum('gap', ['GP0', 'GP1'], outer_class=root_module['ns3::LteRrcSap::MeasGapConfig'])
    ## lte-rrc-sap.h (module 'lte'): ns3::LteRrcSap::MeasIdToAddMod [struct]
    module.add_class('MeasIdToAddMod', outer_class=root_module['ns3::LteRrcSap'])
    ## lte-rrc-sap.h (module 'lte'): ns3::LteRrcSap::MeasObjectEutra [struct]
    module.add_class('MeasObjectEutra', outer_class=root_module['ns3::LteRrcSap'])
    ## lte-rrc-sap.h (module 'lte'): ns3::LteRrcSap::MeasObjectToAddMod [struct]
    module.add_class('MeasObjectToAddMod', outer_class=root_module['ns3::LteRrcSap'])
    ## lte-rrc-sap.h (module 'lte'): ns3::LteRrcSap::MeasResultBestNeighCell [struct]
    module.add_class('MeasResultBestNeighCell', outer_class=root_module['ns3::LteRrcSap'])
    ## lte-rrc-sap.h (module 'lte'): ns3::LteRrcSap::MeasResultEutra [struct]
    module.add_class('MeasResultEutra', outer_class=root_module['ns3::LteRrcSap'])
    ## lte-rrc-sap.h (module 'lte'): ns3::LteRrcSap::MeasResultScell [struct]
    module.add_class('MeasResultScell', outer_class=root_module['ns3::LteRrcSap'])
    ## lte-rrc-sap.h (module 'lte'): ns3::LteRrcSap::MeasResultServFreqList [struct]
    module.add_class('MeasResultServFreqList', outer_class=root_module['ns3::LteRrcSap'])
    ## lte-rrc-sap.h (module 'lte'): ns3::LteRrcSap::MeasResults [struct]
    module.add_class('MeasResults', outer_class=root_module['ns3::LteRrcSap'])
    ## lte-rrc-sap.h (module 'lte'): ns3::LteRrcSap::MeasurementReport [struct]
    module.add_class('MeasurementReport', outer_class=root_module['ns3::LteRrcSap'])
    ## lte-rrc-sap.h (module 'lte'): ns3::LteRrcSap::MobilityControlInfo [struct]
    module.add_class('MobilityControlInfo', outer_class=root_module['ns3::LteRrcSap'])
    ## lte-rrc-sap.h (module 'lte'): ns3::LteRrcSap::MobilityStateParameters [struct]
    module.add_class('MobilityStateParameters', outer_class=root_module['ns3::LteRrcSap'])
    ## lte-rrc-sap.h (module 'lte'): ns3::LteRrcSap::NonCriticalExtensionConfiguration [struct]
    module.add_class('NonCriticalExtensionConfiguration', outer_class=root_module['ns3::LteRrcSap'])
    ## lte-rrc-sap.h (module 'lte'): ns3::LteRrcSap::NonUlConfiguration [struct]
    module.add_class('NonUlConfiguration', outer_class=root_module['ns3::LteRrcSap'])
    ## lte-rrc-sap.h (module 'lte'): ns3::LteRrcSap::PdschConfigCommon [struct]
    module.add_class('PdschConfigCommon', outer_class=root_module['ns3::LteRrcSap'])
    ## lte-rrc-sap.h (module 'lte'): ns3::LteRrcSap::PdschConfigDedicated [struct]
    module.add_class('PdschConfigDedicated', outer_class=root_module['ns3::LteRrcSap'])
    ## lte-rrc-sap.h (module 'lte'): ns3::LteRrcSap::PdschConfigDedicated::db [enumeration]
    module.add_enum('db', ['dB_6', 'dB_4dot77', 'dB_3', 'dB_1dot77', 'dB0', 'dB1', 'dB2', 'dB3'], outer_class=root_module['ns3::LteRrcSap::PdschConfigDedicated'])
    ## lte-rrc-sap.h (module 'lte'): ns3::LteRrcSap::PeriodicBsrTimer [struct]
    module.add_class('PeriodicBsrTimer', outer_class=root_module['ns3::LteRrcSap'])
    ## lte-rrc-sap.h (module 'lte'): ns3::LteRrcSap::PeriodicBsrTimer [enumeration]
    module.add_enum('', ['sf5', 'sf10', 'sf16', 'sf20', 'sf32', 'sf40', 'sf64', 'sf80', 'sf128', 'sf160', 'sf320', 'sf640', 'sf1280', 'sf2560', 'infinity'], outer_class=root_module['ns3::LteRrcSap::PeriodicBsrTimer'])
    ## lte-rrc-sap.h (module 'lte'): ns3::LteRrcSap::PhysCellIdRange [struct]
    module.add_class('PhysCellIdRange', outer_class=root_module['ns3::LteRrcSap'])
    ## lte-rrc-sap.h (module 'lte'): ns3::LteRrcSap::PhysicalConfigDedicated [struct]
    module.add_class('PhysicalConfigDedicated', outer_class=root_module['ns3::LteRrcSap'])
    ## lte-rrc-sap.h (module 'lte'): ns3::LteRrcSap::PhysicalConfigDedicatedSCell [struct]
    module.add_class('PhysicalConfigDedicatedSCell', outer_class=root_module['ns3::LteRrcSap'])
    ## lte-rrc-sap.h (module 'lte'): ns3::LteRrcSap::PlmnIdentityInfo [struct]
    module.add_class('PlmnIdentityInfo', outer_class=root_module['ns3::LteRrcSap'])
    ## lte-rrc-sap.h (module 'lte'): ns3::LteRrcSap::PlmnIdentityList [struct]
    module.add_class('PlmnIdentityList', outer_class=root_module['ns3::LteRrcSap'])
    ## lte-rrc-sap.h (module 'lte'): ns3::LteRrcSap::PoolSelection [struct]
    module.add_class('PoolSelection', outer_class=root_module['ns3::LteRrcSap'])
    ## lte-rrc-sap.h (module 'lte'): ns3::LteRrcSap::PoolSelection [enumeration]
    module.add_enum('', ['RSRPBASED', 'RANDOM'], outer_class=root_module['ns3::LteRrcSap::PoolSelection'])
    ## lte-rrc-sap.h (module 'lte'): ns3::LteRrcSap::PoolSelectionRsrpBased [struct]
    module.add_class('PoolSelectionRsrpBased', outer_class=root_module['ns3::LteRrcSap'])
    ## lte-rrc-sap.h (module 'lte'): ns3::LteRrcSap::PrachConfigSCell [struct]
    module.add_class('PrachConfigSCell', outer_class=root_module['ns3::LteRrcSap'])
    ## lte-rrc-sap.h (module 'lte'): ns3::LteRrcSap::PreambleInfo [struct]
    module.add_class('PreambleInfo', outer_class=root_module['ns3::LteRrcSap'])
    ## lte-rrc-sap.h (module 'lte'): ns3::LteRrcSap::PuschConfigDedicatedSCell [struct]
    module.add_class('PuschConfigDedicatedSCell', outer_class=root_module['ns3::LteRrcSap'])
    ## lte-rrc-sap.h (module 'lte'): ns3::LteRrcSap::QuantityConfig [struct]
    module.add_class('QuantityConfig', outer_class=root_module['ns3::LteRrcSap'])
    ## lte-rrc-sap.h (module 'lte'): ns3::LteRrcSap::RaSupervisionInfo [struct]
    module.add_class('RaSupervisionInfo', outer_class=root_module['ns3::LteRrcSap'])
    ## lte-rrc-sap.h (module 'lte'): ns3::LteRrcSap::RachConfigCommon [struct]
    module.add_class('RachConfigCommon', outer_class=root_module['ns3::LteRrcSap'])
    ## lte-rrc-sap.h (module 'lte'): ns3::LteRrcSap::RachConfigDedicated [struct]
    module.add_class('RachConfigDedicated', outer_class=root_module['ns3::LteRrcSap'])
    ## lte-rrc-sap.h (module 'lte'): ns3::LteRrcSap::RadioResourceConfigCommon [struct]
    module.add_class('RadioResourceConfigCommon', outer_class=root_module['ns3::LteRrcSap'])
    ## lte-rrc-sap.h (module 'lte'): ns3::LteRrcSap::RadioResourceConfigCommonSCell [struct]
    module.add_class('RadioResourceConfigCommonSCell', outer_class=root_module['ns3::LteRrcSap'])
    ## lte-rrc-sap.h (module 'lte'): ns3::LteRrcSap::RadioResourceConfigCommonSib [struct]
    module.add_class('RadioResourceConfigCommonSib', outer_class=root_module['ns3::LteRrcSap'])
    ## lte-rrc-sap.h (module 'lte'): ns3::LteRrcSap::RadioResourceConfigDedicated [struct]
    module.add_class('RadioResourceConfigDedicated', outer_class=root_module['ns3::LteRrcSap'])
    ## lte-rrc-sap.h (module 'lte'): ns3::LteRrcSap::RadioResourceConfigDedicatedSCell [struct]
    module.add_class('RadioResourceConfigDedicatedSCell', outer_class=root_module['ns3::LteRrcSap'])
    ## lte-rrc-sap.h (module 'lte'): ns3::LteRrcSap::ReestabUeIdentity [struct]
    module.add_class('ReestabUeIdentity', outer_class=root_module['ns3::LteRrcSap'])
    ## lte-rrc-sap.h (module 'lte'): ns3::LteRrcSap::ReportConfigEutra [struct]
    module.add_class('ReportConfigEutra', outer_class=root_module['ns3::LteRrcSap'])
    ## lte-rrc-sap.h (module 'lte'): ns3::LteRrcSap::ReportConfigEutra [enumeration]
    module.add_enum('', ['EVENT', 'PERIODICAL'], outer_class=root_module['ns3::LteRrcSap::ReportConfigEutra'])
    ## lte-rrc-sap.h (module 'lte'): ns3::LteRrcSap::ReportConfigEutra [enumeration]
    module.add_enum('', ['EVENT_A1', 'EVENT_A2', 'EVENT_A3', 'EVENT_A4', 'EVENT_A5'], outer_class=root_module['ns3::LteRrcSap::ReportConfigEutra'])
    ## lte-rrc-sap.h (module 'lte'): ns3::LteRrcSap::ReportConfigEutra::report [enumeration]
    module.add_enum('report', ['REPORT_STRONGEST_CELLS', 'REPORT_CGI'], outer_class=root_module['ns3::LteRrcSap::ReportConfigEutra'])
    ## lte-rrc-sap.h (module 'lte'): ns3::LteRrcSap::ReportConfigEutra [enumeration]
    module.add_enum('', ['RSRP', 'RSRQ'], outer_class=root_module['ns3::LteRrcSap::ReportConfigEutra'])
    ## lte-rrc-sap.h (module 'lte'): ns3::LteRrcSap::ReportConfigEutra [enumeration]
    module.add_enum('', ['SAME_AS_TRIGGER_QUANTITY', 'BOTH'], outer_class=root_module['ns3::LteRrcSap::ReportConfigEutra'])
    ## lte-rrc-sap.h (module 'lte'): ns3::LteRrcSap::ReportConfigEutra [enumeration]
    module.add_enum('', ['MS120', 'MS240', 'MS480', 'MS640', 'MS1024', 'MS2048', 'MS5120', 'MS10240', 'MIN1', 'MIN6', 'MIN12', 'MIN30', 'MIN60', 'SPARE3', 'SPARE2', 'SPARE1'], outer_class=root_module['ns3::LteRrcSap::ReportConfigEutra'])
    ## lte-rrc-sap.h (module 'lte'): ns3::LteRrcSap::ReportConfigToAddMod [struct]
    module.add_class('ReportConfigToAddMod', outer_class=root_module['ns3::LteRrcSap'])
    ## lte-rrc-sap.h (module 'lte'): ns3::LteRrcSap::RetxBsrTimer [struct]
    module.add_class('RetxBsrTimer', outer_class=root_module['ns3::LteRrcSap'])
    ## lte-rrc-sap.h (module 'lte'): ns3::LteRrcSap::RetxBsrTimer [enumeration]
    module.add_enum('', ['sf320', 'sf640', 'sf1280', 'sf2560', 'sf5120', 'sf10240'], outer_class=root_module['ns3::LteRrcSap::RetxBsrTimer'])
    ## lte-rrc-sap.h (module 'lte'): ns3::LteRrcSap::RlcConfig [struct]
    module.add_class('RlcConfig', outer_class=root_module['ns3::LteRrcSap'])
    ## lte-rrc-sap.h (module 'lte'): ns3::LteRrcSap::RlcConfig::direction [enumeration]
    module.add_enum('direction', ['AM', 'UM_BI_DIRECTIONAL', 'UM_UNI_DIRECTIONAL_UL', 'UM_UNI_DIRECTIONAL_DL'], outer_class=root_module['ns3::LteRrcSap::RlcConfig'])
    ## lte-rrc-sap.h (module 'lte'): ns3::LteRrcSap::RrcConnectionReconfiguration [struct]
    module.add_class('RrcConnectionReconfiguration', outer_class=root_module['ns3::LteRrcSap'])
    ## lte-rrc-sap.h (module 'lte'): ns3::LteRrcSap::RrcConnectionReconfigurationCompleted [struct]
    module.add_class('RrcConnectionReconfigurationCompleted', outer_class=root_module['ns3::LteRrcSap'])
    ## lte-rrc-sap.h (module 'lte'): ns3::LteRrcSap::RrcConnectionReestablishment [struct]
    module.add_class('RrcConnectionReestablishment', outer_class=root_module['ns3::LteRrcSap'])
    ## lte-rrc-sap.h (module 'lte'): ns3::LteRrcSap::RrcConnectionReestablishmentComplete [struct]
    module.add_class('RrcConnectionReestablishmentComplete', outer_class=root_module['ns3::LteRrcSap'])
    ## lte-rrc-sap.h (module 'lte'): ns3::LteRrcSap::RrcConnectionReestablishmentReject [struct]
    module.add_class('RrcConnectionReestablishmentReject', outer_class=root_module['ns3::LteRrcSap'])
    ## lte-rrc-sap.h (module 'lte'): ns3::LteRrcSap::RrcConnectionReestablishmentRequest [struct]
    module.add_class('RrcConnectionReestablishmentRequest', outer_class=root_module['ns3::LteRrcSap'])
    ## lte-rrc-sap.h (module 'lte'): ns3::LteRrcSap::RrcConnectionReject [struct]
    module.add_class('RrcConnectionReject', outer_class=root_module['ns3::LteRrcSap'])
    ## lte-rrc-sap.h (module 'lte'): ns3::LteRrcSap::RrcConnectionRelease [struct]
    module.add_class('RrcConnectionRelease', outer_class=root_module['ns3::LteRrcSap'])
    ## lte-rrc-sap.h (module 'lte'): ns3::LteRrcSap::RrcConnectionRequest [struct]
    module.add_class('RrcConnectionRequest', outer_class=root_module['ns3::LteRrcSap'])
    ## lte-rrc-sap.h (module 'lte'): ns3::LteRrcSap::RrcConnectionSetup [struct]
    module.add_class('RrcConnectionSetup', outer_class=root_module['ns3::LteRrcSap'])
    ## lte-rrc-sap.h (module 'lte'): ns3::LteRrcSap::RrcConnectionSetupCompleted [struct]
    module.add_class('RrcConnectionSetupCompleted', outer_class=root_module['ns3::LteRrcSap'])
    ## lte-rrc-sap.h (module 'lte'): ns3::LteRrcSap::SCellToAddMod [struct]
    module.add_class('SCellToAddMod', outer_class=root_module['ns3::LteRrcSap'])
    ## lte-rrc-sap.h (module 'lte'): ns3::LteRrcSap::Sib18CommConfig [struct]
    module.add_class('Sib18CommConfig', outer_class=root_module['ns3::LteRrcSap'])
    ## lte-rrc-sap.h (module 'lte'): ns3::LteRrcSap::Sib19DiscConfig [struct]
    module.add_class('Sib19DiscConfig', outer_class=root_module['ns3::LteRrcSap'])
    ## lte-rrc-sap.h (module 'lte'): ns3::LteRrcSap::SidelinkUeInformation [struct]
    module.add_class('SidelinkUeInformation', outer_class=root_module['ns3::LteRrcSap'])
    ## lte-rrc-sap.h (module 'lte'): ns3::LteRrcSap::SlCarrierFreqInfoList [struct]
    module.add_class('SlCarrierFreqInfoList', outer_class=root_module['ns3::LteRrcSap'])
    ## lte-rrc-sap.h (module 'lte'): ns3::LteRrcSap::SlCommConfig [struct]
    module.add_class('SlCommConfig', outer_class=root_module['ns3::LteRrcSap'])
    ## lte-rrc-sap.h (module 'lte'): ns3::LteRrcSap::SlCommConfig [enumeration]
    module.add_enum('', ['RELEASE', 'SETUP'], outer_class=root_module['ns3::LteRrcSap::SlCommConfig'])
    ## lte-rrc-sap.h (module 'lte'): ns3::LteRrcSap::SlCommConfigScheduled [struct]
    module.add_class('SlCommConfigScheduled', outer_class=root_module['ns3::LteRrcSap'])
    ## lte-rrc-sap.h (module 'lte'): ns3::LteRrcSap::SlCommConfigUeSelected [struct]
    module.add_class('SlCommConfigUeSelected', outer_class=root_module['ns3::LteRrcSap'])
    ## lte-rrc-sap.h (module 'lte'): ns3::LteRrcSap::SlCommResourcePool [struct]
    module.add_class('SlCommResourcePool', outer_class=root_module['ns3::LteRrcSap'])
    ## lte-rrc-sap.h (module 'lte'): ns3::LteRrcSap::SlCommResourcePool::TxParameters [struct]
    module.add_class('TxParameters', outer_class=root_module['ns3::LteRrcSap::SlCommResourcePool'])
    ## lte-rrc-sap.h (module 'lte'): ns3::LteRrcSap::SlCommResourcePool::UeSelectedResourceConfig [struct]
    module.add_class('UeSelectedResourceConfig', outer_class=root_module['ns3::LteRrcSap::SlCommResourcePool'])
    ## lte-rrc-sap.h (module 'lte'): ns3::LteRrcSap::SlCommRxPoolList [struct]
    module.add_class('SlCommRxPoolList', outer_class=root_module['ns3::LteRrcSap'])
    ## lte-rrc-sap.h (module 'lte'): ns3::LteRrcSap::SlCommTxPoolList [struct]
    module.add_class('SlCommTxPoolList', outer_class=root_module['ns3::LteRrcSap'])
    ## lte-rrc-sap.h (module 'lte'): ns3::LteRrcSap::SlCommTxPoolToAddMod [struct]
    module.add_class('SlCommTxPoolToAddMod', outer_class=root_module['ns3::LteRrcSap'])
    ## lte-rrc-sap.h (module 'lte'): ns3::LteRrcSap::SlCommTxPoolToAddModList [struct]
    module.add_class('SlCommTxPoolToAddModList', outer_class=root_module['ns3::LteRrcSap'])
    ## lte-rrc-sap.h (module 'lte'): ns3::LteRrcSap::SlCommTxResourceReq [struct]
    module.add_class('SlCommTxResourceReq', outer_class=root_module['ns3::LteRrcSap'])
    ## lte-rrc-sap.h (module 'lte'): ns3::LteRrcSap::SlCommTxResourcesSetup [struct]
    module.add_class('SlCommTxResourcesSetup', outer_class=root_module['ns3::LteRrcSap'])
    ## lte-rrc-sap.h (module 'lte'): ns3::LteRrcSap::SlCommTxResourcesSetup [enumeration]
    module.add_enum('', ['SCHEDULED', 'UE_SELECTED'], outer_class=root_module['ns3::LteRrcSap::SlCommTxResourcesSetup'])
    ## lte-rrc-sap.h (module 'lte'): ns3::LteRrcSap::SlCpLen [struct]
    module.add_class('SlCpLen', outer_class=root_module['ns3::LteRrcSap'])
    ## lte-rrc-sap.h (module 'lte'): ns3::LteRrcSap::SlCpLen [enumeration]
    module.add_enum('', ['NORMAL', 'EXTENDED'], outer_class=root_module['ns3::LteRrcSap::SlCpLen'])
    ## lte-rrc-sap.h (module 'lte'): ns3::LteRrcSap::SlDestinationInfoList [struct]
    module.add_class('SlDestinationInfoList', outer_class=root_module['ns3::LteRrcSap'])
    ## lte-rrc-sap.h (module 'lte'): ns3::LteRrcSap::SlDiscConfig [struct]
    module.add_class('SlDiscConfig', outer_class=root_module['ns3::LteRrcSap'])
    ## lte-rrc-sap.h (module 'lte'): ns3::LteRrcSap::SlDiscConfig [enumeration]
    module.add_enum('', ['RELEASE', 'SETUP'], outer_class=root_module['ns3::LteRrcSap::SlDiscConfig'])
    ## lte-rrc-sap.h (module 'lte'): ns3::LteRrcSap::SlDiscConfigScheduled [struct]
    module.add_class('SlDiscConfigScheduled', outer_class=root_module['ns3::LteRrcSap'])
    ## lte-rrc-sap.h (module 'lte'): ns3::LteRrcSap::SlDiscConfigUeSelected [struct]
    module.add_class('SlDiscConfigUeSelected', outer_class=root_module['ns3::LteRrcSap'])
    ## lte-rrc-sap.h (module 'lte'): ns3::LteRrcSap::SlDiscResourcePool [struct]
    module.add_class('SlDiscResourcePool', outer_class=root_module['ns3::LteRrcSap'])
    ## lte-rrc-sap.h (module 'lte'): ns3::LteRrcSap::SlDiscResourcePool::RxParameters [struct]
    module.add_class('RxParameters', outer_class=root_module['ns3::LteRrcSap::SlDiscResourcePool'])
    ## lte-rrc-sap.h (module 'lte'): ns3::LteRrcSap::SlDiscResourcePool::RxParameters::TddConfig [struct]
    module.add_class('TddConfig', outer_class=root_module['ns3::LteRrcSap::SlDiscResourcePool::RxParameters'])
    ## lte-rrc-sap.h (module 'lte'): ns3::LteRrcSap::SlDiscResourcePool::TxParameters [struct]
    module.add_class('TxParameters', outer_class=root_module['ns3::LteRrcSap::SlDiscResourcePool'])
    ## lte-rrc-sap.h (module 'lte'): ns3::LteRrcSap::SlDiscResourcePool::TxParameters::UeSelectedResourceConfig [struct]
    module.add_class('UeSelectedResourceConfig', outer_class=root_module['ns3::LteRrcSap::SlDiscResourcePool::TxParameters'])
    ## lte-rrc-sap.h (module 'lte'): ns3::LteRrcSap::SlDiscRxPoolList [struct]
    module.add_class('SlDiscRxPoolList', outer_class=root_module['ns3::LteRrcSap'])
    ## lte-rrc-sap.h (module 'lte'): ns3::LteRrcSap::SlDiscTxPoolList [struct]
    module.add_class('SlDiscTxPoolList', outer_class=root_module['ns3::LteRrcSap'])
    ## lte-rrc-sap.h (module 'lte'): ns3::LteRrcSap::SlDiscTxPoolToAddMod [struct]
    module.add_class('SlDiscTxPoolToAddMod', outer_class=root_module['ns3::LteRrcSap'])
    ## lte-rrc-sap.h (module 'lte'): ns3::LteRrcSap::SlDiscTxPoolToAddModList [struct]
    module.add_class('SlDiscTxPoolToAddModList', outer_class=root_module['ns3::LteRrcSap'])
    ## lte-rrc-sap.h (module 'lte'): ns3::LteRrcSap::SlDiscTxPowerInfo [struct]
    module.add_class('SlDiscTxPowerInfo', outer_class=root_module['ns3::LteRrcSap'])
    ## lte-rrc-sap.h (module 'lte'): ns3::LteRrcSap::SlDiscTxPowerInfoList [struct]
    module.add_class('SlDiscTxPowerInfoList', outer_class=root_module['ns3::LteRrcSap'])
    ## lte-rrc-sap.h (module 'lte'): ns3::LteRrcSap::SlDiscTxResourcesSetup [struct]
    module.add_class('SlDiscTxResourcesSetup', outer_class=root_module['ns3::LteRrcSap'])
    ## lte-rrc-sap.h (module 'lte'): ns3::LteRrcSap::SlDiscTxResourcesSetup [enumeration]
    module.add_enum('', ['SCHEDULED', 'UE_SELECTED'], outer_class=root_module['ns3::LteRrcSap::SlDiscTxResourcesSetup'])
    ## lte-rrc-sap.h (module 'lte'): ns3::LteRrcSap::SlHoppingConfigComm [struct]
    module.add_class('SlHoppingConfigComm', outer_class=root_module['ns3::LteRrcSap'])
    ## lte-rrc-sap.h (module 'lte'): ns3::LteRrcSap::SlHoppingConfigComm [enumeration]
    module.add_enum('', ['ns1', 'ns2', 'ns4'], outer_class=root_module['ns3::LteRrcSap::SlHoppingConfigComm'])
    ## lte-rrc-sap.h (module 'lte'): ns3::LteRrcSap::SlHoppingConfigDisc [struct]
    module.add_class('SlHoppingConfigDisc', outer_class=root_module['ns3::LteRrcSap'])
    ## lte-rrc-sap.h (module 'lte'): ns3::LteRrcSap::SlHoppingConfigDisc [enumeration]
    module.add_enum('', ['n1', 'n5'], outer_class=root_module['ns3::LteRrcSap::SlHoppingConfigDisc'])
    ## lte-rrc-sap.h (module 'lte'): ns3::LteRrcSap::SlMacMainConfigSl [struct]
    module.add_class('SlMacMainConfigSl', outer_class=root_module['ns3::LteRrcSap'])
    ## lte-rrc-sap.h (module 'lte'): ns3::LteRrcSap::SlOffsetIndicator [struct]
    module.add_class('SlOffsetIndicator', outer_class=root_module['ns3::LteRrcSap'])
    ## lte-rrc-sap.h (module 'lte'): ns3::LteRrcSap::SlPeriodComm [struct]
    module.add_class('SlPeriodComm', outer_class=root_module['ns3::LteRrcSap'])
    ## lte-rrc-sap.h (module 'lte'): ns3::LteRrcSap::SlPeriodComm [enumeration]
    module.add_enum('', ['sf40', 'sf60', 'sf70', 'sf80', 'sf120', 'sf140', 'sf160', 'sf240', 'sf280', 'sf320'], outer_class=root_module['ns3::LteRrcSap::SlPeriodComm'])
    ## lte-rrc-sap.h (module 'lte'): ns3::LteRrcSap::SlPeriodDisc [struct]
    module.add_class('SlPeriodDisc', outer_class=root_module['ns3::LteRrcSap'])
    ## lte-rrc-sap.h (module 'lte'): ns3::LteRrcSap::SlPeriodDisc [enumeration]
    module.add_enum('', ['rf32', 'rf64', 'rf128', 'rf256', 'rf512', 'rf1024'], outer_class=root_module['ns3::LteRrcSap::SlPeriodDisc'])
    ## lte-rrc-sap.h (module 'lte'): ns3::LteRrcSap::SlPreconfigCommPool [struct]
    module.add_class('SlPreconfigCommPool', outer_class=root_module['ns3::LteRrcSap'])
    ## lte-rrc-sap.h (module 'lte'): ns3::LteRrcSap::SlPreconfigCommPoolList [struct]
    module.add_class('SlPreconfigCommPoolList', outer_class=root_module['ns3::LteRrcSap'])
    ## lte-rrc-sap.h (module 'lte'): ns3::LteRrcSap::SlPreconfigDiscPool [struct]
    module.add_class('SlPreconfigDiscPool', outer_class=root_module['ns3::LteRrcSap'])
    ## lte-rrc-sap.h (module 'lte'): ns3::LteRrcSap::SlPreconfigDiscPool::TxParameters [struct]
    module.add_class('TxParameters', outer_class=root_module['ns3::LteRrcSap::SlPreconfigDiscPool'])
    ## lte-rrc-sap.h (module 'lte'): ns3::LteRrcSap::SlPreconfigDiscPoolList [struct]
    module.add_class('SlPreconfigDiscPoolList', outer_class=root_module['ns3::LteRrcSap'])
    ## lte-rrc-sap.h (module 'lte'): ns3::LteRrcSap::SlPreconfigGeneral [struct]
    module.add_class('SlPreconfigGeneral', outer_class=root_module['ns3::LteRrcSap'])
    ## lte-rrc-sap.h (module 'lte'): ns3::LteRrcSap::SlPreconfigSync [struct]
    module.add_class('SlPreconfigSync', outer_class=root_module['ns3::LteRrcSap'])
    ## lte-rrc-sap.h (module 'lte'): ns3::LteRrcSap::SlPreconfiguration [struct]
    module.add_class('SlPreconfiguration', outer_class=root_module['ns3::LteRrcSap'])
    ## lte-rrc-sap.h (module 'lte'): ns3::LteRrcSap::SlSyncConfigList [struct]
    module.add_class('SlSyncConfigList', outer_class=root_module['ns3::LteRrcSap'])
    ## lte-rrc-sap.h (module 'lte'): ns3::LteRrcSap::SlTfIndexPair [struct]
    module.add_class('SlTfIndexPair', outer_class=root_module['ns3::LteRrcSap'])
    ## lte-rrc-sap.h (module 'lte'): ns3::LteRrcSap::SlTfIndexPairList [struct]
    module.add_class('SlTfIndexPairList', outer_class=root_module['ns3::LteRrcSap'])
    ## lte-rrc-sap.h (module 'lte'): ns3::LteRrcSap::SlTfResourceConfig [struct]
    module.add_class('SlTfResourceConfig', outer_class=root_module['ns3::LteRrcSap'])
    ## lte-rrc-sap.h (module 'lte'): ns3::LteRrcSap::SlTrptSubset [struct]
    module.add_class('SlTrptSubset', outer_class=root_module['ns3::LteRrcSap'])
    ## lte-rrc-sap.h (module 'lte'): ns3::LteRrcSap::SlTxParameters [struct]
    module.add_class('SlTxParameters', outer_class=root_module['ns3::LteRrcSap'])
    ## lte-rrc-sap.h (module 'lte'): ns3::LteRrcSap::SlTxParameters [enumeration]
    module.add_enum('', ['al0', 'al04', 'al05', 'al06', 'al07', 'al08', 'al09', 'al1'], outer_class=root_module['ns3::LteRrcSap::SlTxParameters'])
    ## lte-rrc-sap.h (module 'lte'): ns3::LteRrcSap::SlTxPoolToReleaseList [struct]
    module.add_class('SlTxPoolToReleaseList', outer_class=root_module['ns3::LteRrcSap'])
    ## lte-rrc-sap.h (module 'lte'): ns3::LteRrcSap::SoundingRsUlConfigCommon [struct]
    module.add_class('SoundingRsUlConfigCommon', outer_class=root_module['ns3::LteRrcSap'])
    ## lte-rrc-sap.h (module 'lte'): ns3::LteRrcSap::SoundingRsUlConfigCommon::action [enumeration]
    module.add_enum('action', ['SETUP', 'RESET'], outer_class=root_module['ns3::LteRrcSap::SoundingRsUlConfigCommon'])
    ## lte-rrc-sap.h (module 'lte'): ns3::LteRrcSap::SoundingRsUlConfigDedicated [struct]
    module.add_class('SoundingRsUlConfigDedicated', outer_class=root_module['ns3::LteRrcSap'])
    ## lte-rrc-sap.h (module 'lte'): ns3::LteRrcSap::SoundingRsUlConfigDedicated::action [enumeration]
    module.add_enum('action', ['SETUP', 'RESET'], outer_class=root_module['ns3::LteRrcSap::SoundingRsUlConfigDedicated'])
    ## lte-rrc-sap.h (module 'lte'): ns3::LteRrcSap::SpecialSubframePatterns [struct]
    module.add_class('SpecialSubframePatterns', outer_class=root_module['ns3::LteRrcSap'])
    ## lte-rrc-sap.h (module 'lte'): ns3::LteRrcSap::SpecialSubframePatterns [enumeration]
    module.add_enum('', ['ssp0', 'ssp1', 'ssp2', 'ssp3', 'ssp4', 'ssp5', 'ssp6', 'ssp7', 'ssp8'], outer_class=root_module['ns3::LteRrcSap::SpecialSubframePatterns'])
    ## lte-rrc-sap.h (module 'lte'): ns3::LteRrcSap::SpeedStatePars [struct]
    module.add_class('SpeedStatePars', outer_class=root_module['ns3::LteRrcSap'])
    ## lte-rrc-sap.h (module 'lte'): ns3::LteRrcSap::SpeedStatePars::action [enumeration]
    module.add_enum('action', ['SETUP', 'RESET'], outer_class=root_module['ns3::LteRrcSap::SpeedStatePars'])
    ## lte-rrc-sap.h (module 'lte'): ns3::LteRrcSap::SpeedStateScaleFactors [struct]
    module.add_class('SpeedStateScaleFactors', outer_class=root_module['ns3::LteRrcSap'])
    ## lte-rrc-sap.h (module 'lte'): ns3::LteRrcSap::SrbToAddMod [struct]
    module.add_class('SrbToAddMod', outer_class=root_module['ns3::LteRrcSap'])
    ## lte-rrc-sap.h (module 'lte'): ns3::LteRrcSap::SubframeAssignment [struct]
    module.add_class('SubframeAssignment', outer_class=root_module['ns3::LteRrcSap'])
    ## lte-rrc-sap.h (module 'lte'): ns3::LteRrcSap::SubframeAssignment [enumeration]
    module.add_enum('', ['sa0', 'sa1', 'sa2', 'sa3', 'sa4', 'sa5', 'sa6'], outer_class=root_module['ns3::LteRrcSap::SubframeAssignment'])
    ## lte-rrc-sap.h (module 'lte'): ns3::LteRrcSap::SubframeBitmapSl [struct]
    module.add_class('SubframeBitmapSl', outer_class=root_module['ns3::LteRrcSap'])
    ## lte-rrc-sap.h (module 'lte'): ns3::LteRrcSap::SystemInformation [struct]
    module.add_class('SystemInformation', outer_class=root_module['ns3::LteRrcSap'])
    ## lte-rrc-sap.h (module 'lte'): ns3::LteRrcSap::SystemInformationBlockType1 [struct]
    module.add_class('SystemInformationBlockType1', outer_class=root_module['ns3::LteRrcSap'])
    ## lte-rrc-sap.h (module 'lte'): ns3::LteRrcSap::SystemInformationBlockType18 [struct]
    module.add_class('SystemInformationBlockType18', outer_class=root_module['ns3::LteRrcSap'])
    ## lte-rrc-sap.h (module 'lte'): ns3::LteRrcSap::SystemInformationBlockType19 [struct]
    module.add_class('SystemInformationBlockType19', outer_class=root_module['ns3::LteRrcSap'])
    ## lte-rrc-sap.h (module 'lte'): ns3::LteRrcSap::SystemInformationBlockType2 [struct]
    module.add_class('SystemInformationBlockType2', outer_class=root_module['ns3::LteRrcSap'])
    ## lte-rrc-sap.h (module 'lte'): ns3::LteRrcSap::ThresholdEutra [struct]
    module.add_class('ThresholdEutra', outer_class=root_module['ns3::LteRrcSap'])
    ## lte-rrc-sap.h (module 'lte'): ns3::LteRrcSap::ThresholdEutra [enumeration]
    module.add_enum('', ['THRESHOLD_RSRP', 'THRESHOLD_RSRQ'], outer_class=root_module['ns3::LteRrcSap::ThresholdEutra'])
    ## lte-rrc-sap.h (module 'lte'): ns3::LteRrcSap::TxFailParam [struct]
    module.add_class('TxFailParam', outer_class=root_module['ns3::LteRrcSap'])
    ## lte-rrc-sap.h (module 'lte'): ns3::LteRrcSap::TxProbability [struct]
    module.add_class('TxProbability', outer_class=root_module['ns3::LteRrcSap'])
    ## lte-rrc-sap.h (module 'lte'): ns3::LteRrcSap::TxProbability [enumeration]
    module.add_enum('', ['p25', 'p50', 'p75', 'p100'], outer_class=root_module['ns3::LteRrcSap::TxProbability'])
    ## lte-rrc-sap.h (module 'lte'): ns3::LteRrcSap::UlConfiguration [struct]
    module.add_class('UlConfiguration', outer_class=root_module['ns3::LteRrcSap'])
    ## lte-rrc-sap.h (module 'lte'): ns3::LteRrcSap::UlPowerControlCommonSCell [struct]
    module.add_class('UlPowerControlCommonSCell', outer_class=root_module['ns3::LteRrcSap'])
    ## lte-rrc-sap.h (module 'lte'): ns3::LteRrcSap::UlPowerControlDedicatedSCell [struct]
    module.add_class('UlPowerControlDedicatedSCell', outer_class=root_module['ns3::LteRrcSap'])
    ## lte-sl-pool-factory.h (module 'lte'): ns3::LteSlPoolFactory [class]
    module.add_class('LteSlPoolFactory')
    ## lte-sl-resource-pool-factory.h (module 'lte'): ns3::LteSlResourcePoolFactory [class]
    module.add_class('LteSlResourcePoolFactory', parent=root_module['ns3::LteSlPoolFactory'])
    ## lte-ue-cphy-sap.h (module 'lte'): ns3::LteSlSyncParams [struct]
    module.add_class('LteSlSyncParams')
    ## lte-chunk-processor.h (module 'lte'): ns3::LteSpectrumValueCatcher [class]
    module.add_class('LteSpectrumValueCatcher')
    ## lte-spectrum-value-helper.h (module 'lte'): ns3::LteSpectrumValueHelper [class]
    module.add_class('LteSpectrumValueHelper')
    ## lte-ue-ccm-rrc-sap.h (module 'lte'): ns3::LteUeCcmRrcSapProvider [class]
    module.add_class('LteUeCcmRrcSapProvider', allow_subclassing=True)
    ## lte-ue-ccm-rrc-sap.h (module 'lte'): ns3::LteUeCcmRrcSapProvider::LcsConfig [struct]
    module.add_class('LcsConfig', outer_class=root_module['ns3::LteUeCcmRrcSapProvider'])
    ## lte-ue-ccm-rrc-sap.h (module 'lte'): ns3::LteUeCcmRrcSapUser [class]
    module.add_class('LteUeCcmRrcSapUser', allow_subclassing=True)
    ## lte-ue-cmac-sap.h (module 'lte'): ns3::LteUeCmacSapProvider [class]
    module.add_class('LteUeCmacSapProvider', allow_subclassing=True)
    ## lte-ue-cmac-sap.h (module 'lte'): ns3::LteUeCmacSapProvider::LogicalChannelConfig [struct]
    module.add_class('LogicalChannelConfig', outer_class=root_module['ns3::LteUeCmacSapProvider'])
    ## lte-ue-cmac-sap.h (module 'lte'): ns3::LteUeCmacSapProvider::RachConfig [struct]
    module.add_class('RachConfig', outer_class=root_module['ns3::LteUeCmacSapProvider'])
    ## lte-ue-cmac-sap.h (module 'lte'): ns3::LteUeCmacSapUser [class]
    module.add_class('LteUeCmacSapUser', allow_subclassing=True)
    ## lte-common.h (module 'lte'): ns3::LteUeConfig_t [struct]
    module.add_class('LteUeConfig_t')
    ## lte-ue-cphy-sap.h (module 'lte'): ns3::LteUeCphySapProvider [class]
    module.add_class('LteUeCphySapProvider', allow_subclassing=True)
    ## lte-ue-cphy-sap.h (module 'lte'): ns3::LteUeCphySapUser [class]
    module.add_class('LteUeCphySapUser', allow_subclassing=True)
    ## lte-ue-cphy-sap.h (module 'lte'): ns3::LteUeCphySapUser::UeMeasurementsElement [struct]
    module.add_class('UeMeasurementsElement', outer_class=root_module['ns3::LteUeCphySapUser'])
    ## lte-ue-cphy-sap.h (module 'lte'): ns3::LteUeCphySapUser::UeMeasurementsParameters [struct]
    module.add_class('UeMeasurementsParameters', outer_class=root_module['ns3::LteUeCphySapUser'])
    ## lte-ue-cphy-sap.h (module 'lte'): ns3::LteUeCphySapUser::UeSlssMeasurementsElement [struct]
    module.add_class('UeSlssMeasurementsElement', outer_class=root_module['ns3::LteUeCphySapUser'])
    ## lte-ue-cphy-sap.h (module 'lte'): ns3::LteUeCphySapUser::UeSlssMeasurementsParameters [struct]
    module.add_class('UeSlssMeasurementsParameters', outer_class=root_module['ns3::LteUeCphySapUser'])
    ## lte-ue-phy-sap.h (module 'lte'): ns3::LteUePhySapProvider [class]
    module.add_class('LteUePhySapProvider', allow_subclassing=True)
    ## lte-ue-phy-sap.h (module 'lte'): ns3::LteUePhySapProvider::TransmitSlPhySduParameters [struct]
    module.add_class('TransmitSlPhySduParameters', outer_class=root_module['ns3::LteUePhySapProvider'])
    ## lte-ue-phy-sap.h (module 'lte'): ns3::LteUePhySapProvider::TransmitSlPhySduParameters::SidelinkChannel [enumeration]
    module.add_enum('SidelinkChannel', ['PSBCH', 'PSCCH', 'PSSCH', 'PSDCH', 'INVALID_CH'], outer_class=root_module['ns3::LteUePhySapProvider::TransmitSlPhySduParameters'])
    ## lte-ue-phy-sap.h (module 'lte'): ns3::LteUePhySapUser [class]
    module.add_class('LteUePhySapUser', allow_subclassing=True)
    ## lte-rrc-sap.h (module 'lte'): ns3::LteUeRrcSapProvider [class]
    module.add_class('LteUeRrcSapProvider', parent=root_module['ns3::LteRrcSap'])
    ## lte-rrc-sap.h (module 'lte'): ns3::LteUeRrcSapProvider::CompleteSetupParameters [struct]
    module.add_class('CompleteSetupParameters', outer_class=root_module['ns3::LteUeRrcSapProvider'])
    ## lte-rrc-sap.h (module 'lte'): ns3::LteUeRrcSapUser [class]
    module.add_class('LteUeRrcSapUser', parent=root_module['ns3::LteRrcSap'])
    ## lte-rrc-sap.h (module 'lte'): ns3::LteUeRrcSapUser::SetupParameters [struct]
    module.add_class('SetupParameters', outer_class=root_module['ns3::LteUeRrcSapUser'])
    ## mac48-address.h (module 'network'): ns3::Mac48Address [class]
    module.add_class('Mac48Address', import_from_module='ns.network')
    typehandlers.add_type_alias(u'void ( * ) ( ns3::Mac48Address )', u'ns3::Mac48Address::TracedCallback')
    typehandlers.add_type_alias(u'void ( * ) ( ns3::Mac48Address )*', u'ns3::Mac48Address::TracedCallback*')
    typehandlers.add_type_alias(u'void ( * ) ( ns3::Mac48Address )&', u'ns3::Mac48Address::TracedCallback&')
    ## mac48-address.h (module 'network'): ns3::Mac48Address [class]
    root_module['ns3::Mac48Address'].implicitly_converts_to(root_module['ns3::Address'])
    ## mac64-address.h (module 'network'): ns3::Mac64Address [class]
    module.add_class('Mac64Address', import_from_module='ns.network')
    ## mac64-address.h (module 'network'): ns3::Mac64Address [class]
    root_module['ns3::Mac64Address'].implicitly_converts_to(root_module['ns3::Address'])
    ## mac8-address.h (module 'network'): ns3::Mac8Address [class]
    module.add_class('Mac8Address', import_from_module='ns.network')
    ## mac8-address.h (module 'network'): ns3::Mac8Address [class]
    root_module['ns3::Mac8Address'].implicitly_converts_to(root_module['ns3::Address'])
    ## ff-mac-common.h (module 'lte'): ns3::MacCeListElement_s [struct]
    module.add_class('MacCeListElement_s')
    ## ff-mac-common.h (module 'lte'): ns3::MacCeListElement_s::MacCeType_e [enumeration]
    module.add_enum('MacCeType_e', ['BSR', 'PHR', 'CRNTI', 'SLBSR'], outer_class=root_module['ns3::MacCeListElement_s'])
    ## ff-mac-common.h (module 'lte'): ns3::MacCeValue_u [struct]
    module.add_class('MacCeValue_u')
    ## names.h (module 'core'): ns3::Names [class]
    module.add_class('Names', import_from_module='ns.core')
    ## net-device-container.h (module 'network'): ns3::NetDeviceContainer [class]
    module.add_class('NetDeviceContainer', import_from_module='ns.network')
    typehandlers.add_type_alias(u'std::vector< ns3::Ptr< ns3::NetDevice > > const_iterator', u'ns3::NetDeviceContainer::Iterator')
    typehandlers.add_type_alias(u'std::vector< ns3::Ptr< ns3::NetDevice > > const_iterator*', u'ns3::NetDeviceContainer::Iterator*')
    typehandlers.add_type_alias(u'std::vector< ns3::Ptr< ns3::NetDevice > > const_iterator&', u'ns3::NetDeviceContainer::Iterator&')
    ## node-container.h (module 'network'): ns3::NodeContainer [class]
    module.add_class('NodeContainer', import_from_module='ns.network')
    typehandlers.add_type_alias(u'std::vector< ns3::Ptr< ns3::Node > > const_iterator', u'ns3::NodeContainer::Iterator')
    typehandlers.add_type_alias(u'std::vector< ns3::Ptr< ns3::Node > > const_iterator*', u'ns3::NodeContainer::Iterator*')
    typehandlers.add_type_alias(u'std::vector< ns3::Ptr< ns3::Node > > const_iterator&', u'ns3::NodeContainer::Iterator&')
    ## object-base.h (module 'core'): ns3::ObjectBase [class]
    module.add_class('ObjectBase', allow_subclassing=True, import_from_module='ns.core')
    ## object.h (module 'core'): ns3::ObjectDeleter [struct]
    module.add_class('ObjectDeleter', import_from_module='ns.core')
    ## object-factory.h (module 'core'): ns3::ObjectFactory [class]
    module.add_class('ObjectFactory', import_from_module='ns.core')
    ## packet-metadata.h (module 'network'): ns3::PacketMetadata [class]
    module.add_class('PacketMetadata', import_from_module='ns.network')
    ## packet-metadata.h (module 'network'): ns3::PacketMetadata::Item [struct]
    module.add_class('Item', import_from_module='ns.network', outer_class=root_module['ns3::PacketMetadata'])
    ## packet-metadata.h (module 'network'): ns3::PacketMetadata::Item::ItemType [enumeration]
    module.add_enum('ItemType', ['PAYLOAD', 'HEADER', 'TRAILER'], outer_class=root_module['ns3::PacketMetadata::Item'], import_from_module='ns.network')
    ## packet-metadata.h (module 'network'): ns3::PacketMetadata::ItemIterator [class]
    module.add_class('ItemIterator', import_from_module='ns.network', outer_class=root_module['ns3::PacketMetadata'])
    ## packet.h (module 'network'): ns3::PacketTagIterator [class]
    module.add_class('PacketTagIterator', import_from_module='ns.network')
    ## packet.h (module 'network'): ns3::PacketTagIterator::Item [class]
    module.add_class('Item', import_from_module='ns.network', outer_class=root_module['ns3::PacketTagIterator'])
    ## packet-tag-list.h (module 'network'): ns3::PacketTagList [class]
    module.add_class('PacketTagList', import_from_module='ns.network')
    ## packet-tag-list.h (module 'network'): ns3::PacketTagList::TagData [struct]
    module.add_class('TagData', import_from_module='ns.network', outer_class=root_module['ns3::PacketTagList'])
    ## ff-mac-common.h (module 'lte'): ns3::PagingInfoListElement_s [struct]
    module.add_class('PagingInfoListElement_s')
    ## log.h (module 'core'): ns3::ParameterLogger [class]
    module.add_class('ParameterLogger', import_from_module='ns.core')
    ## ff-mac-common.h (module 'lte'): ns3::PhichListElement_s [struct]
    module.add_class('PhichListElement_s')
    ## ff-mac-common.h (module 'lte'): ns3::PhichListElement_s::Phich_e [enumeration]
    module.add_enum('Phich_e', ['ACK', 'NACK'], outer_class=root_module['ns3::PhichListElement_s'])
    ## lte-common.h (module 'lte'): ns3::PhyReceptionStatParameters [struct]
    module.add_class('PhyReceptionStatParameters')
    typehandlers.add_type_alias(u'void ( * ) ( ns3::PhyReceptionStatParameters const )', u'ns3::PhyReceptionStatParameters::TracedCallback')
    typehandlers.add_type_alias(u'void ( * ) ( ns3::PhyReceptionStatParameters const )*', u'ns3::PhyReceptionStatParameters::TracedCallback*')
    typehandlers.add_type_alias(u'void ( * ) ( ns3::PhyReceptionStatParameters const )&', u'ns3::PhyReceptionStatParameters::TracedCallback&')
    ## lte-common.h (module 'lte'): ns3::PhyTransmissionStatParameters [struct]
    module.add_class('PhyTransmissionStatParameters')
    typehandlers.add_type_alias(u'void ( * ) ( ns3::PhyTransmissionStatParameters const )', u'ns3::PhyTransmissionStatParameters::TracedCallback')
    typehandlers.add_type_alias(u'void ( * ) ( ns3::PhyTransmissionStatParameters const )*', u'ns3::PhyTransmissionStatParameters::TracedCallback*')
    typehandlers.add_type_alias(u'void ( * ) ( ns3::PhyTransmissionStatParameters const )&', u'ns3::PhyTransmissionStatParameters::TracedCallback&')
    ## ff-mac-common.h (module 'lte'): ns3::RachListElement_s [struct]
    module.add_class('RachListElement_s')
    ## radio-bearer-stats-connector.h (module 'lte'): ns3::RadioBearerStatsConnector [class]
    module.add_class('RadioBearerStatsConnector')
    ## lte-rrc-protocol-real.h (module 'lte'): ns3::RealProtocolRlcSapUser [class]
    module.add_class('RealProtocolRlcSapUser', parent=root_module['ns3::LteRlcSapUser'])
    ## ff-mac-common.h (module 'lte'): ns3::RlcPduListElement_s [struct]
    module.add_class('RlcPduListElement_s')
    ## ff-mac-common.h (module 'lte'): ns3::SbMeasResult_s [struct]
    module.add_class('SbMeasResult_s')
    ## ff-mac-common.h (module 'lte'): ns3::SciListElement_s [struct]
    module.add_class('SciListElement_s')
    ## lte-rlc-sequence-number.h (module 'lte'): ns3::SequenceNumber10 [class]
    module.add_class('SequenceNumber10')
    ## ff-mac-common.h (module 'lte'): ns3::SiConfiguration_s [struct]
    module.add_class('SiConfiguration_s')
    ## ff-mac-common.h (module 'lte'): ns3::SiMessageListElement_s [struct]
    module.add_class('SiMessageListElement_s')
    ## simple-ref-count.h (module 'core'): ns3::SimpleRefCount<ns3::Object, ns3::ObjectBase, ns3::ObjectDeleter> [class]
    module.add_class('SimpleRefCount', automatic_type_narrowing=True, import_from_module='ns.core', template_parameters=['ns3::Object', 'ns3::ObjectBase', 'ns3::ObjectDeleter'], parent=root_module['ns3::ObjectBase'], memory_policy=cppclass.ReferenceCountingMethodsPolicy(incref_method='Ref', decref_method='Unref', peekref_method='GetReferenceCount'))
    ## simulator.h (module 'core'): ns3::Simulator [class]
    module.add_class('Simulator', destructor_visibility='private', import_from_module='ns.core')
    ## simulator.h (module 'core'): ns3::Simulator [enumeration]
    module.add_enum('', ['NO_CONTEXT'], outer_class=root_module['ns3::Simulator'], import_from_module='ns.core')
    ## lte-spectrum-phy.h (module 'lte'): ns3::SlCtrlPacketInfo_t [struct]
    module.add_class('SlCtrlPacketInfo_t')
    ## ff-mac-common.h (module 'lte'): ns3::SlDciListElement_s [struct]
    module.add_class('SlDciListElement_s')
    ## lte-spectrum-phy.h (module 'lte'): ns3::SlDiscTbId_t [struct]
    module.add_class('SlDiscTbId_t')
    ## lte-spectrum-phy.h (module 'lte'): ns3::SlDisctbInfo_t [struct]
    module.add_class('SlDisctbInfo_t')
    ## lte-common.h (module 'lte'): ns3::SlPhyReceptionStatParameters [struct]
    module.add_class('SlPhyReceptionStatParameters')
    typehandlers.add_type_alias(u'void ( * ) ( ns3::SlPhyReceptionStatParameters const )', u'ns3::SlPhyReceptionStatParameters::TracedCallback')
    typehandlers.add_type_alias(u'void ( * ) ( ns3::SlPhyReceptionStatParameters const )*', u'ns3::SlPhyReceptionStatParameters::TracedCallback*')
    typehandlers.add_type_alias(u'void ( * ) ( ns3::SlPhyReceptionStatParameters const )&', u'ns3::SlPhyReceptionStatParameters::TracedCallback&')
    ## lte-common.h (module 'lte'): ns3::SlPhyTransmissionStatParameters [struct]
    module.add_class('SlPhyTransmissionStatParameters')
    typehandlers.add_type_alias(u'void ( * ) ( ns3::SlPhyTransmissionStatParameters const )', u'ns3::SlPhyTransmissionStatParameters::TracedCallback')
    typehandlers.add_type_alias(u'void ( * ) ( ns3::SlPhyTransmissionStatParameters const )*', u'ns3::SlPhyTransmissionStatParameters::TracedCallback*')
    typehandlers.add_type_alias(u'void ( * ) ( ns3::SlPhyTransmissionStatParameters const )&', u'ns3::SlPhyTransmissionStatParameters::TracedCallback&')
    ## lte-spectrum-phy.h (module 'lte'): ns3::SlRxPacketInfo_t [struct]
    module.add_class('SlRxPacketInfo_t')
    ## lte-spectrum-phy.h (module 'lte'): ns3::SlTbId_t [struct]
    module.add_class('SlTbId_t')
    ## lte-common.h (module 'lte'): ns3::SlUeMacStatParameters [struct]
    module.add_class('SlUeMacStatParameters')
    typehandlers.add_type_alias(u'void ( * ) ( ns3::SlUeMacStatParameters const )', u'ns3::SlUeMacStatParameters::TracedCallback')
    typehandlers.add_type_alias(u'void ( * ) ( ns3::SlUeMacStatParameters const )*', u'ns3::SlUeMacStatParameters::TracedCallback*')
    typehandlers.add_type_alias(u'void ( * ) ( ns3::SlUeMacStatParameters const )&', u'ns3::SlUeMacStatParameters::TracedCallback&')
    ## lte-spectrum-phy.h (module 'lte'): ns3::SltbInfo_t [struct]
    module.add_class('SltbInfo_t')
    ## ff-mac-common.h (module 'lte'): ns3::SpsConfig_s [struct]
    module.add_class('SpsConfig_s')
    ## ff-mac-common.h (module 'lte'): ns3::SrConfig_s [struct]
    module.add_class('SrConfig_s')
    ## ff-mac-common.h (module 'lte'): ns3::SrListElement_s [struct]
    module.add_class('SrListElement_s')
    ## data-calculator.h (module 'stats'): ns3::StatisticalSummary [class]
    module.add_class('StatisticalSummary', allow_subclassing=True, import_from_module='ns.stats')
    ## tag.h (module 'network'): ns3::Tag [class]
    module.add_class('Tag', import_from_module='ns.network', parent=root_module['ns3::ObjectBase'])
    ## tag-buffer.h (module 'network'): ns3::TagBuffer [class]
    module.add_class('TagBuffer', import_from_module='ns.network')
    ## lte-nist-error-model.h (module 'lte'): ns3::TbErrorStats_t [struct]
    module.add_class('TbErrorStats_t')
    ## lte-spectrum-phy.h (module 'lte'): ns3::TbId_t [struct]
    module.add_class('TbId_t')
    ## lte-mi-error-model.h (module 'lte'): ns3::TbStats_t [struct]
    module.add_class('TbStats_t')
    ## nstime.h (module 'core'): ns3::TimeWithUnit [class]
    module.add_class('TimeWithUnit', import_from_module='ns.core')
    ## lte-common.h (module 'lte'): ns3::TransmissionModesLayers [class]
    module.add_class('TransmissionModesLayers')
    ## type-id.h (module 'core'): ns3::TypeId [class]
    module.add_class('TypeId', import_from_module='ns.core')
    ## type-id.h (module 'core'): ns3::TypeId::AttributeFlag [enumeration]
    module.add_enum('AttributeFlag', ['ATTR_GET', 'ATTR_SET', 'ATTR_CONSTRUCT', 'ATTR_SGC'], outer_class=root_module['ns3::TypeId'], import_from_module='ns.core')
    ## type-id.h (module 'core'): ns3::TypeId::SupportLevel [enumeration]
    module.add_enum('SupportLevel', ['SUPPORTED', 'DEPRECATED', 'OBSOLETE'], outer_class=root_module['ns3::TypeId'], import_from_module='ns.core')
    ## type-id.h (module 'core'): ns3::TypeId::AttributeInformation [struct]
    module.add_class('AttributeInformation', import_from_module='ns.core', outer_class=root_module['ns3::TypeId'])
    ## type-id.h (module 'core'): ns3::TypeId::TraceSourceInformation [struct]
    module.add_class('TraceSourceInformation', import_from_module='ns.core', outer_class=root_module['ns3::TypeId'])
    typehandlers.add_type_alias(u'uint32_t', u'ns3::TypeId::hash_t')
    typehandlers.add_type_alias(u'uint32_t*', u'ns3::TypeId::hash_t*')
    typehandlers.add_type_alias(u'uint32_t&', u'ns3::TypeId::hash_t&')
    ## ff-mac-common.h (module 'lte'): ns3::UeCapabilities_s [struct]
    module.add_class('UeCapabilities_s')
    ## ff-mac-common.h (module 'lte'): ns3::UeSelected_s [struct]
    module.add_class('UeSelected_s')
    ## ff-mac-common.h (module 'lte'): ns3::UlCqi_s [struct]
    module.add_class('UlCqi_s')
    ## ff-mac-common.h (module 'lte'): ns3::UlCqi_s::Type_e [enumeration]
    module.add_enum('Type_e', ['SRS', 'PUSCH', 'PUCCH_1', 'PUCCH_2', 'PRACH'], outer_class=root_module['ns3::UlCqi_s'])
    ## ff-mac-common.h (module 'lte'): ns3::UlDciListElement_s [struct]
    module.add_class('UlDciListElement_s')
    ## ff-mac-common.h (module 'lte'): ns3::UlGrant_s [struct]
    module.add_class('UlGrant_s')
    ## ff-mac-common.h (module 'lte'): ns3::UlInfoListElement_s [struct]
    module.add_class('UlInfoListElement_s')
    ## ff-mac-common.h (module 'lte'): ns3::UlInfoListElement_s::ReceptionStatus_e [enumeration]
    module.add_enum('ReceptionStatus_e', ['Ok', 'NotOk', 'NotValid'], outer_class=root_module['ns3::UlInfoListElement_s'])
    ## lte-global-pathloss-database.h (module 'lte'): ns3::UplinkLteGlobalPathlossDatabase [class]
    module.add_class('UplinkLteGlobalPathlossDatabase', parent=root_module['ns3::LteGlobalPathlossDatabase'])
    ## vector.h (module 'core'): ns3::Vector2D [class]
    module.add_class('Vector2D', import_from_module='ns.core')
    ## vector.h (module 'core'): ns3::Vector3D [class]
    module.add_class('Vector3D', import_from_module='ns.core')
    ## ff-mac-common.h (module 'lte'): ns3::VendorSpecificListElement_s [struct]
    module.add_class('VendorSpecificListElement_s')
    ## lte-3gpp-hex-grid-enb-topology-helper.h (module 'lte'): ns3::WrapAroundInfo_t [struct]
    module.add_class('WrapAroundInfo_t')
    ## lte-3gpp-hex-grid-enb-topology-helper.h (module 'lte'): ns3::WrapAroundReplicas [struct]
    module.add_class('WrapAroundReplicas')
    ## empty.h (module 'core'): ns3::empty [class]
    module.add_class('empty', import_from_module='ns.core')
    ## fdbet-ff-mac-scheduler.h (module 'lte'): ns3::fdbetsFlowPerf_t [struct]
    module.add_class('fdbetsFlowPerf_t')
    ## fdtbfq-ff-mac-scheduler.h (module 'lte'): ns3::fdtbfqsFlowPerf_t [struct]
    module.add_class('fdtbfqsFlowPerf_t')
    ## int64x64-128.h (module 'core'): ns3::int64x64_t [class]
    module.add_class('int64x64_t', import_from_module='ns.core')
    ## int64x64-128.h (module 'core'): ns3::int64x64_t::impl_type [enumeration]
    module.add_enum('impl_type', ['int128_impl', 'cairo_impl', 'ld_impl'], outer_class=root_module['ns3::int64x64_t'], import_from_module='ns.core')
    ## pf-ff-mac-scheduler.h (module 'lte'): ns3::pfsFlowPerf_t [struct]
    module.add_class('pfsFlowPerf_t')
    ## pss-ff-mac-scheduler.h (module 'lte'): ns3::pssFlowPerf_t [struct]
    module.add_class('pssFlowPerf_t')
    ## lte-spectrum-phy.h (module 'lte'): ns3::tbInfo_t [struct]
    module.add_class('tbInfo_t')
    ## tdbet-ff-mac-scheduler.h (module 'lte'): ns3::tdbetsFlowPerf_t [struct]
    module.add_class('tdbetsFlowPerf_t')
    ## tdtbfq-ff-mac-scheduler.h (module 'lte'): ns3::tdtbfqsFlowPerf_t [struct]
    module.add_class('tdtbfqsFlowPerf_t')
    ## chunk.h (module 'network'): ns3::Chunk [class]
    module.add_class('Chunk', import_from_module='ns.network', parent=root_module['ns3::ObjectBase'])
    ## lte-global-pathloss-database.h (module 'lte'): ns3::DownlinkLteGlobalPathlossDatabase [class]
    module.add_class('DownlinkLteGlobalPathlossDatabase', parent=root_module['ns3::LteGlobalPathlossDatabase'])
    ## eps-bearer.h (module 'lte'): ns3::EpsBearer [class]
    module.add_class('EpsBearer', parent=root_module['ns3::ObjectBase'])
    ## eps-bearer.h (module 'lte'): ns3::EpsBearer::Qci [enumeration]
    module.add_enum('Qci', ['GBR_CONV_VOICE', 'GBR_CONV_VIDEO', 'GBR_GAMING', 'GBR_NON_CONV_VIDEO', 'GBR_MC_PUSH_TO_TALK', 'GBR_NMC_PUSH_TO_TALK', 'GBR_MC_VIDEO', 'GBR_V2X', 'NGBR_IMS', 'NGBR_VIDEO_TCP_OPERATOR', 'NGBR_VOICE_VIDEO_GAMING', 'NGBR_VIDEO_TCP_PREMIUM', 'NGBR_VIDEO_TCP_DEFAULT', 'NGBR_MC_DELAY_SIGNAL', 'NGBR_MC_DATA', 'NGBR_V2X', 'NGBR_LOW_LAT_EMBB', 'DGBR_DISCRETE_AUT_SMALL', 'DGBR_DISCRETE_AUT_LARGE', 'DGBR_ITS', 'DGBR_ELECTRICITY'], outer_class=root_module['ns3::EpsBearer'])
    ## eps-bearer-tag.h (module 'lte'): ns3::EpsBearerTag [class]
    module.add_class('EpsBearerTag', parent=root_module['ns3::Tag'])
    ## header.h (module 'network'): ns3::Header [class]
    module.add_class('Header', import_from_module='ns.network', parent=root_module['ns3::Chunk'])
    ## ipv4-header.h (module 'internet'): ns3::Ipv4Header [class]
    module.add_class('Ipv4Header', import_from_module='ns.internet', parent=root_module['ns3::Header'])
    ## ipv4-header.h (module 'internet'): ns3::Ipv4Header::DscpType [enumeration]
    module.add_enum('DscpType', ['DscpDefault', 'DSCP_CS1', 'DSCP_AF11', 'DSCP_AF12', 'DSCP_AF13', 'DSCP_CS2', 'DSCP_AF21', 'DSCP_AF22', 'DSCP_AF23', 'DSCP_CS3', 'DSCP_AF31', 'DSCP_AF32', 'DSCP_AF33', 'DSCP_CS4', 'DSCP_AF41', 'DSCP_AF42', 'DSCP_AF43', 'DSCP_CS5', 'DSCP_EF', 'DSCP_CS6', 'DSCP_CS7'], outer_class=root_module['ns3::Ipv4Header'], import_from_module='ns.internet')
    ## ipv4-header.h (module 'internet'): ns3::Ipv4Header::EcnType [enumeration]
    module.add_enum('EcnType', ['ECN_NotECT', 'ECN_ECT1', 'ECN_ECT0', 'ECN_CE'], outer_class=root_module['ns3::Ipv4Header'], import_from_module='ns.internet')
    ## lte-ccm-mac-sap.h (module 'lte'): ns3::LteCcmMacSapUser [class]
    module.add_class('LteCcmMacSapUser', parent=root_module['ns3::LteMacSapUser'])
    ## lte-rrc-sap.h (module 'lte'): ns3::LteEnbRrcSapProvider [class]
    module.add_class('LteEnbRrcSapProvider', parent=root_module['ns3::LteRrcSap'])
    ## lte-rrc-sap.h (module 'lte'): ns3::LteEnbRrcSapProvider::CompleteSetupUeParameters [struct]
    module.add_class('CompleteSetupUeParameters', outer_class=root_module['ns3::LteEnbRrcSapProvider'])
    ## lte-rrc-sap.h (module 'lte'): ns3::LteEnbRrcSapUser [class]
    module.add_class('LteEnbRrcSapUser', parent=root_module['ns3::LteRrcSap'])
    ## lte-rrc-sap.h (module 'lte'): ns3::LteEnbRrcSapUser::SetupUeParameters [struct]
    module.add_class('SetupUeParameters', outer_class=root_module['ns3::LteEnbRrcSapUser'])
    ## lte-pdcp-header.h (module 'lte'): ns3::LtePdcpHeader [class]
    module.add_class('LtePdcpHeader', parent=root_module['ns3::Header'])
    ## lte-pdcp-header.h (module 'lte'): ns3::LtePdcpHeader [enumeration]
    module.add_enum('', ['CONTROL_PDU', 'DATA_PDU'], outer_class=root_module['ns3::LtePdcpHeader'])
    ## lte-phy-tag.h (module 'lte'): ns3::LtePhyTag [class]
    module.add_class('LtePhyTag', parent=root_module['ns3::Tag'])
    ## lte-radio-bearer-tag.h (module 'lte'): ns3::LteRadioBearerTag [class]
    module.add_class('LteRadioBearerTag', parent=root_module['ns3::Tag'])
    ## lte-rlc-am-header.h (module 'lte'): ns3::LteRlcAmHeader [class]
    module.add_class('LteRlcAmHeader', parent=root_module['ns3::Header'])
    ## lte-rlc-am-header.h (module 'lte'): ns3::LteRlcAmHeader::DataControlPdu_t [enumeration]
    module.add_enum('DataControlPdu_t', ['CONTROL_PDU', 'DATA_PDU'], outer_class=root_module['ns3::LteRlcAmHeader'])
    ## lte-rlc-am-header.h (module 'lte'): ns3::LteRlcAmHeader::ControPduType_t [enumeration]
    module.add_enum('ControPduType_t', ['STATUS_PDU'], outer_class=root_module['ns3::LteRlcAmHeader'])
    ## lte-rlc-am-header.h (module 'lte'): ns3::LteRlcAmHeader::FramingInfoFirstByte_t [enumeration]
    module.add_enum('FramingInfoFirstByte_t', ['FIRST_BYTE', 'NO_FIRST_BYTE'], outer_class=root_module['ns3::LteRlcAmHeader'])
    ## lte-rlc-am-header.h (module 'lte'): ns3::LteRlcAmHeader::FramingInfoLastByte_t [enumeration]
    module.add_enum('FramingInfoLastByte_t', ['LAST_BYTE', 'NO_LAST_BYTE'], outer_class=root_module['ns3::LteRlcAmHeader'])
    ## lte-rlc-am-header.h (module 'lte'): ns3::LteRlcAmHeader::ExtensionBit_t [enumeration]
    module.add_enum('ExtensionBit_t', ['DATA_FIELD_FOLLOWS', 'E_LI_FIELDS_FOLLOWS'], outer_class=root_module['ns3::LteRlcAmHeader'])
    ## lte-rlc-am-header.h (module 'lte'): ns3::LteRlcAmHeader::ResegmentationFlag_t [enumeration]
    module.add_enum('ResegmentationFlag_t', ['PDU', 'SEGMENT'], outer_class=root_module['ns3::LteRlcAmHeader'])
    ## lte-rlc-am-header.h (module 'lte'): ns3::LteRlcAmHeader::PollingBit_t [enumeration]
    module.add_enum('PollingBit_t', ['STATUS_REPORT_NOT_REQUESTED', 'STATUS_REPORT_IS_REQUESTED'], outer_class=root_module['ns3::LteRlcAmHeader'])
    ## lte-rlc-am-header.h (module 'lte'): ns3::LteRlcAmHeader::LastSegmentFlag_t [enumeration]
    module.add_enum('LastSegmentFlag_t', ['NO_LAST_PDU_SEGMENT', 'LAST_PDU_SEGMENT'], outer_class=root_module['ns3::LteRlcAmHeader'])
    typehandlers.add_type_alias(u'ns3::LteRlcAmHeader::DataControlPdu_t', u'ns3::LteRlcAmHeader::DataControlPdu_t')
    typehandlers.add_type_alias(u'ns3::LteRlcAmHeader::DataControlPdu_t*', u'ns3::LteRlcAmHeader::DataControlPdu_t*')
    typehandlers.add_type_alias(u'ns3::LteRlcAmHeader::DataControlPdu_t&', u'ns3::LteRlcAmHeader::DataControlPdu_t&')
    typehandlers.add_type_alias(u'ns3::LteRlcAmHeader::ControPduType_t', u'ns3::LteRlcAmHeader::ControPduType_t')
    typehandlers.add_type_alias(u'ns3::LteRlcAmHeader::ControPduType_t*', u'ns3::LteRlcAmHeader::ControPduType_t*')
    typehandlers.add_type_alias(u'ns3::LteRlcAmHeader::ControPduType_t&', u'ns3::LteRlcAmHeader::ControPduType_t&')
    typehandlers.add_type_alias(u'ns3::LteRlcAmHeader::FramingInfoFirstByte_t', u'ns3::LteRlcAmHeader::FramingInfoFirstByte_t')
    typehandlers.add_type_alias(u'ns3::LteRlcAmHeader::FramingInfoFirstByte_t*', u'ns3::LteRlcAmHeader::FramingInfoFirstByte_t*')
    typehandlers.add_type_alias(u'ns3::LteRlcAmHeader::FramingInfoFirstByte_t&', u'ns3::LteRlcAmHeader::FramingInfoFirstByte_t&')
    typehandlers.add_type_alias(u'ns3::LteRlcAmHeader::FramingInfoLastByte_t', u'ns3::LteRlcAmHeader::FramingInfoLastByte_t')
    typehandlers.add_type_alias(u'ns3::LteRlcAmHeader::FramingInfoLastByte_t*', u'ns3::LteRlcAmHeader::FramingInfoLastByte_t*')
    typehandlers.add_type_alias(u'ns3::LteRlcAmHeader::FramingInfoLastByte_t&', u'ns3::LteRlcAmHeader::FramingInfoLastByte_t&')
    typehandlers.add_type_alias(u'ns3::LteRlcAmHeader::ExtensionBit_t', u'ns3::LteRlcAmHeader::ExtensionBit_t')
    typehandlers.add_type_alias(u'ns3::LteRlcAmHeader::ExtensionBit_t*', u'ns3::LteRlcAmHeader::ExtensionBit_t*')
    typehandlers.add_type_alias(u'ns3::LteRlcAmHeader::ExtensionBit_t&', u'ns3::LteRlcAmHeader::ExtensionBit_t&')
    typehandlers.add_type_alias(u'ns3::LteRlcAmHeader::ResegmentationFlag_t', u'ns3::LteRlcAmHeader::ResegmentationFlag_t')
    typehandlers.add_type_alias(u'ns3::LteRlcAmHeader::ResegmentationFlag_t*', u'ns3::LteRlcAmHeader::ResegmentationFlag_t*')
    typehandlers.add_type_alias(u'ns3::LteRlcAmHeader::ResegmentationFlag_t&', u'ns3::LteRlcAmHeader::ResegmentationFlag_t&')
    typehandlers.add_type_alias(u'ns3::LteRlcAmHeader::PollingBit_t', u'ns3::LteRlcAmHeader::PollingBit_t')
    typehandlers.add_type_alias(u'ns3::LteRlcAmHeader::PollingBit_t*', u'ns3::LteRlcAmHeader::PollingBit_t*')
    typehandlers.add_type_alias(u'ns3::LteRlcAmHeader::PollingBit_t&', u'ns3::LteRlcAmHeader::PollingBit_t&')
    typehandlers.add_type_alias(u'ns3::LteRlcAmHeader::LastSegmentFlag_t', u'ns3::LteRlcAmHeader::LastSegmentFlag_t')
    typehandlers.add_type_alias(u'ns3::LteRlcAmHeader::LastSegmentFlag_t*', u'ns3::LteRlcAmHeader::LastSegmentFlag_t*')
    typehandlers.add_type_alias(u'ns3::LteRlcAmHeader::LastSegmentFlag_t&', u'ns3::LteRlcAmHeader::LastSegmentFlag_t&')
    ## lte-rlc-header.h (module 'lte'): ns3::LteRlcHeader [class]
    module.add_class('LteRlcHeader', parent=root_module['ns3::Header'])
    ## lte-rlc-header.h (module 'lte'): ns3::LteRlcHeader::ExtensionBit_t [enumeration]
    module.add_enum('ExtensionBit_t', ['DATA_FIELD_FOLLOWS', 'E_LI_FIELDS_FOLLOWS'], outer_class=root_module['ns3::LteRlcHeader'])
    ## lte-rlc-header.h (module 'lte'): ns3::LteRlcHeader::FramingInfoFirstByte_t [enumeration]
    module.add_enum('FramingInfoFirstByte_t', ['FIRST_BYTE', 'NO_FIRST_BYTE'], outer_class=root_module['ns3::LteRlcHeader'])
    ## lte-rlc-header.h (module 'lte'): ns3::LteRlcHeader::FramingInfoLastByte_t [enumeration]
    module.add_enum('FramingInfoLastByte_t', ['LAST_BYTE', 'NO_LAST_BYTE'], outer_class=root_module['ns3::LteRlcHeader'])
    typehandlers.add_type_alias(u'ns3::LteRlcHeader::ExtensionBit_t', u'ns3::LteRlcHeader::ExtensionBit_t')
    typehandlers.add_type_alias(u'ns3::LteRlcHeader::ExtensionBit_t*', u'ns3::LteRlcHeader::ExtensionBit_t*')
    typehandlers.add_type_alias(u'ns3::LteRlcHeader::ExtensionBit_t&', u'ns3::LteRlcHeader::ExtensionBit_t&')
    typehandlers.add_type_alias(u'ns3::LteRlcHeader::FramingInfoFirstByte_t', u'ns3::LteRlcHeader::FramingInfoFirstByte_t')
    typehandlers.add_type_alias(u'ns3::LteRlcHeader::FramingInfoFirstByte_t*', u'ns3::LteRlcHeader::FramingInfoFirstByte_t*')
    typehandlers.add_type_alias(u'ns3::LteRlcHeader::FramingInfoFirstByte_t&', u'ns3::LteRlcHeader::FramingInfoFirstByte_t&')
    typehandlers.add_type_alias(u'ns3::LteRlcHeader::FramingInfoLastByte_t', u'ns3::LteRlcHeader::FramingInfoLastByte_t')
    typehandlers.add_type_alias(u'ns3::LteRlcHeader::FramingInfoLastByte_t*', u'ns3::LteRlcHeader::FramingInfoLastByte_t*')
    typehandlers.add_type_alias(u'ns3::LteRlcHeader::FramingInfoLastByte_t&', u'ns3::LteRlcHeader::FramingInfoLastByte_t&')
    ## lte-rlc-sdu-status-tag.h (module 'lte'): ns3::LteRlcSduStatusTag [class]
    module.add_class('LteRlcSduStatusTag', parent=root_module['ns3::Tag'])
    ## lte-rlc-sdu-status-tag.h (module 'lte'): ns3::LteRlcSduStatusTag::SduStatus_t [enumeration]
    module.add_enum('SduStatus_t', ['FULL_SDU', 'FIRST_SEGMENT', 'MIDDLE_SEGMENT', 'LAST_SEGMENT', 'ANY_SEGMENT'], outer_class=root_module['ns3::LteRlcSduStatusTag'])
    typehandlers.add_type_alias(u'ns3::LteRlcSduStatusTag::SduStatus_t', u'ns3::LteRlcSduStatusTag::SduStatus_t')
    typehandlers.add_type_alias(u'ns3::LteRlcSduStatusTag::SduStatus_t*', u'ns3::LteRlcSduStatusTag::SduStatus_t*')
    typehandlers.add_type_alias(u'ns3::LteRlcSduStatusTag::SduStatus_t&', u'ns3::LteRlcSduStatusTag::SduStatus_t&')
    ## lte-sl-header.h (module 'lte'): ns3::LteSlDiscHeader [class]
    module.add_class('LteSlDiscHeader', parent=root_module['ns3::Header'])
    ## lte-sl-header.h (module 'lte'): ns3::LteSlDiscHeader::DiscoveryMsgType [enumeration]
    module.add_enum('DiscoveryMsgType', ['DISC_OPEN_ANNOUNCEMENT', 'DISC_RESTRICTED_ANNOUNCEMENT', 'DISC_RESTRICTED_RESPONSE', 'DISC_RELAY_ANNOUNCEMENT', 'DISC_RELAY_SOLICITATION', 'DISC_RELAY_RESPONSE'], outer_class=root_module['ns3::LteSlDiscHeader'])
    ## lte-sl-disc-resource-pool-factory.h (module 'lte'): ns3::LteSlDiscResourcePoolFactory [class]
    module.add_class('LteSlDiscResourcePoolFactory', parent=root_module['ns3::LteSlPoolFactory'])
    ## lte-sl-preconfig-pool-factory.h (module 'lte'): ns3::LteSlPreconfigPoolFactory [class]
    module.add_class('LteSlPreconfigPoolFactory', parent=root_module['ns3::LteSlResourcePoolFactory'])
    ## lte-sl-header.h (module 'lte'): ns3::LteSlSciHeader [class]
    module.add_class('LteSlSciHeader', parent=root_module['ns3::Header'])
    ## lte-sl-tag.h (module 'lte'): ns3::LteSlSciTag [class]
    module.add_class('LteSlSciTag', parent=root_module['ns3::Tag'])
    ## object.h (module 'core'): ns3::Object [class]
    module.add_class('Object', import_from_module='ns.core', parent=root_module['ns3::SimpleRefCount< ns3::Object, ns3::ObjectBase, ns3::ObjectDeleter >'])
    ## object.h (module 'core'): ns3::Object::AggregateIterator [class]
    module.add_class('AggregateIterator', import_from_module='ns.core', outer_class=root_module['ns3::Object'])
    ## packet-burst.h (module 'network'): ns3::PacketBurst [class]
    module.add_class('PacketBurst', import_from_module='ns.network', parent=root_module['ns3::Object'])
    typehandlers.add_type_alias(u'void ( * ) ( ns3::Ptr< ns3::PacketBurst const > )', u'ns3::PacketBurst::TracedCallback')
    typehandlers.add_type_alias(u'void ( * ) ( ns3::Ptr< ns3::PacketBurst const > )*', u'ns3::PacketBurst::TracedCallback*')
    typehandlers.add_type_alias(u'void ( * ) ( ns3::Ptr< ns3::PacketBurst const > )&', u'ns3::PacketBurst::TracedCallback&')
    ## lte-pdcp-tag.h (module 'lte'): ns3::PdcpTag [class]
    module.add_class('PdcpTag', parent=root_module['ns3::Tag'])
    ## propagation-delay-model.h (module 'propagation'): ns3::PropagationDelayModel [class]
    module.add_class('PropagationDelayModel', import_from_module='ns.propagation', parent=root_module['ns3::Object'])
    ## propagation-loss-model.h (module 'propagation'): ns3::PropagationLossModel [class]
    module.add_class('PropagationLossModel', import_from_module='ns.propagation', parent=root_module['ns3::Object'])
    ## radio-environment-map-helper.h (module 'lte'): ns3::RadioEnvironmentMapHelper [class]
    module.add_class('RadioEnvironmentMapHelper', parent=root_module['ns3::Object'])
    ## propagation-delay-model.h (module 'propagation'): ns3::RandomPropagationDelayModel [class]
    module.add_class('RandomPropagationDelayModel', import_from_module='ns.propagation', parent=root_module['ns3::PropagationDelayModel'])
    ## propagation-loss-model.h (module 'propagation'): ns3::RandomPropagationLossModel [class]
    module.add_class('RandomPropagationLossModel', import_from_module='ns.propagation', parent=root_module['ns3::PropagationLossModel'])
    ## random-variable-stream.h (module 'core'): ns3::RandomVariableStream [class]
    module.add_class('RandomVariableStream', import_from_module='ns.core', parent=root_module['ns3::Object'])
    ## propagation-loss-model.h (module 'propagation'): ns3::RangePropagationLossModel [class]
    module.add_class('RangePropagationLossModel', import_from_module='ns.propagation', parent=root_module['ns3::PropagationLossModel'])
    ## lte-rlc-tag.h (module 'lte'): ns3::RlcTag [class]
    module.add_class('RlcTag', parent=root_module['ns3::Tag'])
    ## random-variable-stream.h (module 'core'): ns3::SequentialRandomVariable [class]
    module.add_class('SequentialRandomVariable', import_from_module='ns.core', parent=root_module['ns3::RandomVariableStream'])
    ## lte-sl-pool.h (module 'lte'): ns3::SidelinkCommResourcePool [class]
    module.add_class('SidelinkCommResourcePool', parent=root_module['ns3::Object'])
    ## lte-sl-pool.h (module 'lte'): ns3::SidelinkCommResourcePool::SlPoolType [enumeration]
    module.add_enum('SlPoolType', ['UNKNOWN', 'SCHEDULED', 'UE_SELECTED'], outer_class=root_module['ns3::SidelinkCommResourcePool'])
    ## lte-sl-pool.h (module 'lte'): ns3::SidelinkCommResourcePool::SidelinkTransmissionInfo [struct]
    module.add_class('SidelinkTransmissionInfo', outer_class=root_module['ns3::SidelinkCommResourcePool'])
    ## lte-sl-pool.h (module 'lte'): ns3::SidelinkCommResourcePool::SubframeInfo [struct]
    module.add_class('SubframeInfo', outer_class=root_module['ns3::SidelinkCommResourcePool'])
    typehandlers.add_type_alias(u'void ( * ) ( uint32_t const, uint32_t const, uint32_t const )', u'ns3::SidelinkCommResourcePool::ScPeriodTracedCallback')
    typehandlers.add_type_alias(u'void ( * ) ( uint32_t const, uint32_t const, uint32_t const )*', u'ns3::SidelinkCommResourcePool::ScPeriodTracedCallback*')
    typehandlers.add_type_alias(u'void ( * ) ( uint32_t const, uint32_t const, uint32_t const )&', u'ns3::SidelinkCommResourcePool::ScPeriodTracedCallback&')
    ## lte-sl-pool.h (module 'lte'): ns3::SidelinkDiscResourcePool [class]
    module.add_class('SidelinkDiscResourcePool', parent=root_module['ns3::Object'])
    ## lte-sl-pool.h (module 'lte'): ns3::SidelinkDiscResourcePool::SlPoolType [enumeration]
    module.add_enum('SlPoolType', ['UNKNOWN', 'SCHEDULED', 'UE_SELECTED'], outer_class=root_module['ns3::SidelinkDiscResourcePool'])
    ## lte-sl-pool.h (module 'lte'): ns3::SidelinkDiscResourcePool::SidelinkTransmissionInfo [struct]
    module.add_class('SidelinkTransmissionInfo', outer_class=root_module['ns3::SidelinkDiscResourcePool'])
    ## lte-sl-pool.h (module 'lte'): ns3::SidelinkDiscResourcePool::SubframeInfo [struct]
    module.add_class('SubframeInfo', outer_class=root_module['ns3::SidelinkDiscResourcePool'])
    ## sidelink-rsrp-calculator.h (module 'lte'): ns3::SidelinkRsrpCalculator [class]
    module.add_class('SidelinkRsrpCalculator', parent=root_module['ns3::Object'])
    ## lte-sl-pool.h (module 'lte'): ns3::SidelinkRxCommResourcePool [class]
    module.add_class('SidelinkRxCommResourcePool', parent=root_module['ns3::SidelinkCommResourcePool'])
    ## lte-sl-pool.h (module 'lte'): ns3::SidelinkRxDiscResourcePool [class]
    module.add_class('SidelinkRxDiscResourcePool', parent=root_module['ns3::SidelinkDiscResourcePool'])
    ## lte-sl-pool.h (module 'lte'): ns3::SidelinkTxCommResourcePool [class]
    module.add_class('SidelinkTxCommResourcePool', parent=root_module['ns3::SidelinkCommResourcePool'])
    ## lte-sl-pool.h (module 'lte'): ns3::SidelinkTxDiscResourcePool [class]
    module.add_class('SidelinkTxDiscResourcePool', parent=root_module['ns3::SidelinkDiscResourcePool'])
    ## simple-ref-count.h (module 'core'): ns3::SimpleRefCount<ns3::AttributeAccessor, ns3::empty, ns3::DefaultDeleter<ns3::AttributeAccessor> > [class]
    module.add_class('SimpleRefCount', automatic_type_narrowing=True, import_from_module='ns.core', template_parameters=['ns3::AttributeAccessor', 'ns3::empty', 'ns3::DefaultDeleter<ns3::AttributeAccessor>'], parent=root_module['ns3::empty'], memory_policy=cppclass.ReferenceCountingMethodsPolicy(incref_method='Ref', decref_method='Unref', peekref_method='GetReferenceCount'))
    ## simple-ref-count.h (module 'core'): ns3::SimpleRefCount<ns3::AttributeChecker, ns3::empty, ns3::DefaultDeleter<ns3::AttributeChecker> > [class]
    module.add_class('SimpleRefCount', automatic_type_narrowing=True, import_from_module='ns.core', template_parameters=['ns3::AttributeChecker', 'ns3::empty', 'ns3::DefaultDeleter<ns3::AttributeChecker>'], parent=root_module['ns3::empty'], memory_policy=cppclass.ReferenceCountingMethodsPolicy(incref_method='Ref', decref_method='Unref', peekref_method='GetReferenceCount'))
    ## simple-ref-count.h (module 'core'): ns3::SimpleRefCount<ns3::AttributeValue, ns3::empty, ns3::DefaultDeleter<ns3::AttributeValue> > [class]
    module.add_class('SimpleRefCount', automatic_type_narrowing=True, import_from_module='ns.core', template_parameters=['ns3::AttributeValue', 'ns3::empty', 'ns3::DefaultDeleter<ns3::AttributeValue>'], parent=root_module['ns3::empty'], memory_policy=cppclass.ReferenceCountingMethodsPolicy(incref_method='Ref', decref_method='Unref', peekref_method='GetReferenceCount'))
    ## simple-ref-count.h (module 'core'): ns3::SimpleRefCount<ns3::CallbackImplBase, ns3::empty, ns3::DefaultDeleter<ns3::CallbackImplBase> > [class]
    module.add_class('SimpleRefCount', automatic_type_narrowing=True, import_from_module='ns.core', template_parameters=['ns3::CallbackImplBase', 'ns3::empty', 'ns3::DefaultDeleter<ns3::CallbackImplBase>'], parent=root_module['ns3::empty'], memory_policy=cppclass.ReferenceCountingMethodsPolicy(incref_method='Ref', decref_method='Unref', peekref_method='GetReferenceCount'))
    ## simple-ref-count.h (module 'core'): ns3::SimpleRefCount<ns3::EpcTft, ns3::empty, ns3::DefaultDeleter<ns3::EpcTft> > [class]
    module.add_class('SimpleRefCount', automatic_type_narrowing=True, template_parameters=['ns3::EpcTft', 'ns3::empty', 'ns3::DefaultDeleter<ns3::EpcTft>'], parent=root_module['ns3::empty'], memory_policy=cppclass.ReferenceCountingMethodsPolicy(incref_method='Ref', decref_method='Unref', peekref_method='GetReferenceCount'))
    ## simple-ref-count.h (module 'core'): ns3::SimpleRefCount<ns3::EpcTftClassifier, ns3::empty, ns3::DefaultDeleter<ns3::EpcTftClassifier> > [class]
    module.add_class('SimpleRefCount', automatic_type_narrowing=True, template_parameters=['ns3::EpcTftClassifier', 'ns3::empty', 'ns3::DefaultDeleter<ns3::EpcTftClassifier>'], parent=root_module['ns3::empty'], memory_policy=cppclass.ReferenceCountingMethodsPolicy(incref_method='Ref', decref_method='Unref', peekref_method='GetReferenceCount'))
    ## simple-ref-count.h (module 'core'): ns3::SimpleRefCount<ns3::EventImpl, ns3::empty, ns3::DefaultDeleter<ns3::EventImpl> > [class]
    module.add_class('SimpleRefCount', automatic_type_narrowing=True, import_from_module='ns.core', template_parameters=['ns3::EventImpl', 'ns3::empty', 'ns3::DefaultDeleter<ns3::EventImpl>'], parent=root_module['ns3::empty'], memory_policy=cppclass.ReferenceCountingMethodsPolicy(incref_method='Ref', decref_method='Unref', peekref_method='GetReferenceCount'))
    ## simple-ref-count.h (module 'core'): ns3::SimpleRefCount<ns3::Hash::Implementation, ns3::empty, ns3::DefaultDeleter<ns3::Hash::Implementation> > [class]
    module.add_class('SimpleRefCount', automatic_type_narrowing=True, import_from_module='ns.core', template_parameters=['ns3::Hash::Implementation', 'ns3::empty', 'ns3::DefaultDeleter<ns3::Hash::Implementation>'], parent=root_module['ns3::empty'], memory_policy=cppclass.ReferenceCountingMethodsPolicy(incref_method='Ref', decref_method='Unref', peekref_method='GetReferenceCount'))
    ## simple-ref-count.h (module 'core'): ns3::SimpleRefCount<ns3::Ipv4MulticastRoute, ns3::empty, ns3::DefaultDeleter<ns3::Ipv4MulticastRoute> > [class]
    module.add_class('SimpleRefCount', automatic_type_narrowing=True, import_from_module='ns.core', template_parameters=['ns3::Ipv4MulticastRoute', 'ns3::empty', 'ns3::DefaultDeleter<ns3::Ipv4MulticastRoute>'], parent=root_module['ns3::empty'], memory_policy=cppclass.ReferenceCountingMethodsPolicy(incref_method='Ref', decref_method='Unref', peekref_method='GetReferenceCount'))
    ## simple-ref-count.h (module 'core'): ns3::SimpleRefCount<ns3::Ipv4Route, ns3::empty, ns3::DefaultDeleter<ns3::Ipv4Route> > [class]
    module.add_class('SimpleRefCount', automatic_type_narrowing=True, import_from_module='ns.core', template_parameters=['ns3::Ipv4Route', 'ns3::empty', 'ns3::DefaultDeleter<ns3::Ipv4Route>'], parent=root_module['ns3::empty'], memory_policy=cppclass.ReferenceCountingMethodsPolicy(incref_method='Ref', decref_method='Unref', peekref_method='GetReferenceCount'))
    ## simple-ref-count.h (module 'core'): ns3::SimpleRefCount<ns3::LteChunkProcessor, ns3::empty, ns3::DefaultDeleter<ns3::LteChunkProcessor> > [class]
    module.add_class('SimpleRefCount', automatic_type_narrowing=True, template_parameters=['ns3::LteChunkProcessor', 'ns3::empty', 'ns3::DefaultDeleter<ns3::LteChunkProcessor>'], parent=root_module['ns3::empty'], memory_policy=cppclass.ReferenceCountingMethodsPolicy(incref_method='Ref', decref_method='Unref', peekref_method='GetReferenceCount'))
    ## simple-ref-count.h (module 'core'): ns3::SimpleRefCount<ns3::LteControlMessage, ns3::empty, ns3::DefaultDeleter<ns3::LteControlMessage> > [class]
    module.add_class('SimpleRefCount', automatic_type_narrowing=True, template_parameters=['ns3::LteControlMessage', 'ns3::empty', 'ns3::DefaultDeleter<ns3::LteControlMessage>'], parent=root_module['ns3::empty'], memory_policy=cppclass.ReferenceCountingMethodsPolicy(incref_method='Ref', decref_method='Unref', peekref_method='GetReferenceCount'))
    ## simple-ref-count.h (module 'core'): ns3::SimpleRefCount<ns3::LteHarqPhy, ns3::empty, ns3::DefaultDeleter<ns3::LteHarqPhy> > [class]
    module.add_class('SimpleRefCount', automatic_type_narrowing=True, template_parameters=['ns3::LteHarqPhy', 'ns3::empty', 'ns3::DefaultDeleter<ns3::LteHarqPhy>'], parent=root_module['ns3::empty'], memory_policy=cppclass.ReferenceCountingMethodsPolicy(incref_method='Ref', decref_method='Unref', peekref_method='GetReferenceCount'))
    ## simple-ref-count.h (module 'core'): ns3::SimpleRefCount<ns3::LteSlChunkProcessor, ns3::empty, ns3::DefaultDeleter<ns3::LteSlChunkProcessor> > [class]
    module.add_class('SimpleRefCount', automatic_type_narrowing=True, template_parameters=['ns3::LteSlChunkProcessor', 'ns3::empty', 'ns3::DefaultDeleter<ns3::LteSlChunkProcessor>'], parent=root_module['ns3::empty'], memory_policy=cppclass.ReferenceCountingMethodsPolicy(incref_method='Ref', decref_method='Unref', peekref_method='GetReferenceCount'))
    ## simple-ref-count.h (module 'core'): ns3::SimpleRefCount<ns3::LteSlHarqPhy, ns3::empty, ns3::DefaultDeleter<ns3::LteSlHarqPhy> > [class]
    module.add_class('SimpleRefCount', automatic_type_narrowing=True, template_parameters=['ns3::LteSlHarqPhy', 'ns3::empty', 'ns3::DefaultDeleter<ns3::LteSlHarqPhy>'], parent=root_module['ns3::empty'], memory_policy=cppclass.ReferenceCountingMethodsPolicy(incref_method='Ref', decref_method='Unref', peekref_method='GetReferenceCount'))
    ## simple-ref-count.h (module 'core'): ns3::SimpleRefCount<ns3::LteSlTft, ns3::empty, ns3::DefaultDeleter<ns3::LteSlTft> > [class]
    module.add_class('SimpleRefCount', automatic_type_narrowing=True, template_parameters=['ns3::LteSlTft', 'ns3::empty', 'ns3::DefaultDeleter<ns3::LteSlTft>'], parent=root_module['ns3::empty'], memory_policy=cppclass.ReferenceCountingMethodsPolicy(incref_method='Ref', decref_method='Unref', peekref_method='GetReferenceCount'))
    ## simple-ref-count.h (module 'core'): ns3::SimpleRefCount<ns3::NixVector, ns3::empty, ns3::DefaultDeleter<ns3::NixVector> > [class]
    module.add_class('SimpleRefCount', automatic_type_narrowing=True, import_from_module='ns.core', template_parameters=['ns3::NixVector', 'ns3::empty', 'ns3::DefaultDeleter<ns3::NixVector>'], parent=root_module['ns3::empty'], memory_policy=cppclass.ReferenceCountingMethodsPolicy(incref_method='Ref', decref_method='Unref', peekref_method='GetReferenceCount'))
    ## simple-ref-count.h (module 'core'): ns3::SimpleRefCount<ns3::OutputStreamWrapper, ns3::empty, ns3::DefaultDeleter<ns3::OutputStreamWrapper> > [class]
    module.add_class('SimpleRefCount', automatic_type_narrowing=True, import_from_module='ns.core', template_parameters=['ns3::OutputStreamWrapper', 'ns3::empty', 'ns3::DefaultDeleter<ns3::OutputStreamWrapper>'], parent=root_module['ns3::empty'], memory_policy=cppclass.ReferenceCountingMethodsPolicy(incref_method='Ref', decref_method='Unref', peekref_method='GetReferenceCount'))
    ## simple-ref-count.h (module 'core'): ns3::SimpleRefCount<ns3::Packet, ns3::empty, ns3::DefaultDeleter<ns3::Packet> > [class]
    module.add_class('SimpleRefCount', automatic_type_narrowing=True, import_from_module='ns.core', template_parameters=['ns3::Packet', 'ns3::empty', 'ns3::DefaultDeleter<ns3::Packet>'], parent=root_module['ns3::empty'], memory_policy=cppclass.ReferenceCountingMethodsPolicy(incref_method='Ref', decref_method='Unref', peekref_method='GetReferenceCount'))
    ## simple-ref-count.h (module 'core'): ns3::SimpleRefCount<ns3::SpectrumModel, ns3::empty, ns3::DefaultDeleter<ns3::SpectrumModel> > [class]
    module.add_class('SimpleRefCount', automatic_type_narrowing=True, import_from_module='ns.core', template_parameters=['ns3::SpectrumModel', 'ns3::empty', 'ns3::DefaultDeleter<ns3::SpectrumModel>'], parent=root_module['ns3::empty'], memory_policy=cppclass.ReferenceCountingMethodsPolicy(incref_method='Ref', decref_method='Unref', peekref_method='GetReferenceCount'))
    ## simple-ref-count.h (module 'core'): ns3::SimpleRefCount<ns3::SpectrumSignalParameters, ns3::empty, ns3::DefaultDeleter<ns3::SpectrumSignalParameters> > [class]
    module.add_class('SimpleRefCount', automatic_type_narrowing=True, import_from_module='ns.core', template_parameters=['ns3::SpectrumSignalParameters', 'ns3::empty', 'ns3::DefaultDeleter<ns3::SpectrumSignalParameters>'], parent=root_module['ns3::empty'], memory_policy=cppclass.ReferenceCountingMethodsPolicy(incref_method='Ref', decref_method='Unref', peekref_method='GetReferenceCount'))
    ## simple-ref-count.h (module 'core'): ns3::SimpleRefCount<ns3::SpectrumValue, ns3::empty, ns3::DefaultDeleter<ns3::SpectrumValue> > [class]
    module.add_class('SimpleRefCount', automatic_type_narrowing=True, import_from_module='ns.core', template_parameters=['ns3::SpectrumValue', 'ns3::empty', 'ns3::DefaultDeleter<ns3::SpectrumValue>'], parent=root_module['ns3::empty'], memory_policy=cppclass.ReferenceCountingMethodsPolicy(incref_method='Ref', decref_method='Unref', peekref_method='GetReferenceCount'))
    ## simple-ref-count.h (module 'core'): ns3::SimpleRefCount<ns3::TraceSourceAccessor, ns3::empty, ns3::DefaultDeleter<ns3::TraceSourceAccessor> > [class]
    module.add_class('SimpleRefCount', automatic_type_narrowing=True, import_from_module='ns.core', template_parameters=['ns3::TraceSourceAccessor', 'ns3::empty', 'ns3::DefaultDeleter<ns3::TraceSourceAccessor>'], parent=root_module['ns3::empty'], memory_policy=cppclass.ReferenceCountingMethodsPolicy(incref_method='Ref', decref_method='Unref', peekref_method='GetReferenceCount'))
    ## simple-ref-count.h (module 'core'): ns3::SimpleRefCount<ns3::VendorSpecificValue, ns3::empty, ns3::DefaultDeleter<ns3::VendorSpecificValue> > [class]
    module.add_class('SimpleRefCount', automatic_type_narrowing=True, template_parameters=['ns3::VendorSpecificValue', 'ns3::empty', 'ns3::DefaultDeleter<ns3::VendorSpecificValue>'], parent=root_module['ns3::empty'], memory_policy=cppclass.ReferenceCountingMethodsPolicy(incref_method='Ref', decref_method='Unref', peekref_method='GetReferenceCount'))
    ## simple-ref-count.h (module 'core'): ns3::SimpleRefCount<ns3::X2CellInfo, ns3::empty, ns3::DefaultDeleter<ns3::X2CellInfo> > [class]
    module.add_class('SimpleRefCount', automatic_type_narrowing=True, template_parameters=['ns3::X2CellInfo', 'ns3::empty', 'ns3::DefaultDeleter<ns3::X2CellInfo>'], parent=root_module['ns3::empty'], memory_policy=cppclass.ReferenceCountingMethodsPolicy(incref_method='Ref', decref_method='Unref', peekref_method='GetReferenceCount'))
    ## simple-ref-count.h (module 'core'): ns3::SimpleRefCount<ns3::X2IfaceInfo, ns3::empty, ns3::DefaultDeleter<ns3::X2IfaceInfo> > [class]
    module.add_class('SimpleRefCount', automatic_type_narrowing=True, template_parameters=['ns3::X2IfaceInfo', 'ns3::empty', 'ns3::DefaultDeleter<ns3::X2IfaceInfo>'], parent=root_module['ns3::empty'], memory_policy=cppclass.ReferenceCountingMethodsPolicy(incref_method='Ref', decref_method='Unref', peekref_method='GetReferenceCount'))
    ## socket.h (module 'network'): ns3::Socket [class]
    module.add_class('Socket', import_from_module='ns.network', parent=root_module['ns3::Object'])
    ## socket.h (module 'network'): ns3::Socket::SocketErrno [enumeration]
    module.add_enum('SocketErrno', ['ERROR_NOTERROR', 'ERROR_ISCONN', 'ERROR_NOTCONN', 'ERROR_MSGSIZE', 'ERROR_AGAIN', 'ERROR_SHUTDOWN', 'ERROR_OPNOTSUPP', 'ERROR_AFNOSUPPORT', 'ERROR_INVAL', 'ERROR_BADF', 'ERROR_NOROUTETOHOST', 'ERROR_NODEV', 'ERROR_ADDRNOTAVAIL', 'ERROR_ADDRINUSE', 'SOCKET_ERRNO_LAST'], outer_class=root_module['ns3::Socket'], import_from_module='ns.network')
    ## socket.h (module 'network'): ns3::Socket::SocketType [enumeration]
    module.add_enum('SocketType', ['NS3_SOCK_STREAM', 'NS3_SOCK_SEQPACKET', 'NS3_SOCK_DGRAM', 'NS3_SOCK_RAW'], outer_class=root_module['ns3::Socket'], import_from_module='ns.network')
    ## socket.h (module 'network'): ns3::Socket::SocketPriority [enumeration]
    module.add_enum('SocketPriority', ['NS3_PRIO_BESTEFFORT', 'NS3_PRIO_FILLER', 'NS3_PRIO_BULK', 'NS3_PRIO_INTERACTIVE_BULK', 'NS3_PRIO_INTERACTIVE', 'NS3_PRIO_CONTROL'], outer_class=root_module['ns3::Socket'], import_from_module='ns.network')
    ## socket.h (module 'network'): ns3::Socket::Ipv6MulticastFilterMode [enumeration]
    module.add_enum('Ipv6MulticastFilterMode', ['INCLUDE', 'EXCLUDE'], outer_class=root_module['ns3::Socket'], import_from_module='ns.network')
    ## socket.h (module 'network'): ns3::SocketIpTosTag [class]
    module.add_class('SocketIpTosTag', import_from_module='ns.network', parent=root_module['ns3::Tag'])
    ## socket.h (module 'network'): ns3::SocketIpTtlTag [class]
    module.add_class('SocketIpTtlTag', import_from_module='ns.network', parent=root_module['ns3::Tag'])
    ## socket.h (module 'network'): ns3::SocketIpv6HopLimitTag [class]
    module.add_class('SocketIpv6HopLimitTag', import_from_module='ns.network', parent=root_module['ns3::Tag'])
    ## socket.h (module 'network'): ns3::SocketIpv6TclassTag [class]
    module.add_class('SocketIpv6TclassTag', import_from_module='ns.network', parent=root_module['ns3::Tag'])
    ## socket.h (module 'network'): ns3::SocketPriorityTag [class]
    module.add_class('SocketPriorityTag', import_from_module='ns.network', parent=root_module['ns3::Tag'])
    ## socket.h (module 'network'): ns3::SocketSetDontFragmentTag [class]
    module.add_class('SocketSetDontFragmentTag', import_from_module='ns.network', parent=root_module['ns3::Tag'])
    ## spectrum-interference.h (module 'spectrum'): ns3::SpectrumInterference [class]
    module.add_class('SpectrumInterference', import_from_module='ns.spectrum', parent=root_module['ns3::Object'])
    ## spectrum-model.h (module 'spectrum'): ns3::SpectrumModel [class]
    module.add_class('SpectrumModel', import_from_module='ns.spectrum', parent=root_module['ns3::SimpleRefCount< ns3::SpectrumModel, ns3::empty, ns3::DefaultDeleter<ns3::SpectrumModel> >'])
    ## spectrum-phy.h (module 'spectrum'): ns3::SpectrumPhy [class]
    module.add_class('SpectrumPhy', import_from_module='ns.spectrum', parent=root_module['ns3::Object'])
    ## spectrum-propagation-loss-model.h (module 'spectrum'): ns3::SpectrumPropagationLossModel [class]
    module.add_class('SpectrumPropagationLossModel', import_from_module='ns.spectrum', parent=root_module['ns3::Object'])
    ## spectrum-signal-parameters.h (module 'spectrum'): ns3::SpectrumSignalParameters [struct]
    module.add_class('SpectrumSignalParameters', import_from_module='ns.spectrum', parent=root_module['ns3::SimpleRefCount< ns3::SpectrumSignalParameters, ns3::empty, ns3::DefaultDeleter<ns3::SpectrumSignalParameters> >'])
    ## spectrum-value.h (module 'spectrum'): ns3::SpectrumValue [class]
    module.add_class('SpectrumValue', import_from_module='ns.spectrum', parent=root_module['ns3::SimpleRefCount< ns3::SpectrumValue, ns3::empty, ns3::DefaultDeleter<ns3::SpectrumValue> >'])
    typehandlers.add_type_alias(u'void ( * ) ( ns3::Ptr< ns3::SpectrumValue > )', u'ns3::SpectrumValue::TracedCallback')
    typehandlers.add_type_alias(u'void ( * ) ( ns3::Ptr< ns3::SpectrumValue > )*', u'ns3::SpectrumValue::TracedCallback*')
    typehandlers.add_type_alias(u'void ( * ) ( ns3::Ptr< ns3::SpectrumValue > )&', u'ns3::SpectrumValue::TracedCallback&')
    ## propagation-loss-model.h (module 'propagation'): ns3::ThreeLogDistancePropagationLossModel [class]
    module.add_class('ThreeLogDistancePropagationLossModel', import_from_module='ns.propagation', parent=root_module['ns3::PropagationLossModel'])
    ## nstime.h (module 'core'): ns3::Time [class]
    module.add_class('Time', import_from_module='ns.core')
    ## nstime.h (module 'core'): ns3::Time::Unit [enumeration]
    module.add_enum('Unit', ['Y', 'D', 'H', 'MIN', 'S', 'MS', 'US', 'NS', 'PS', 'FS', 'LAST'], outer_class=root_module['ns3::Time'], import_from_module='ns.core')
    typehandlers.add_type_alias(u'void ( * ) ( ns3::Time )', u'ns3::Time::TracedCallback')
    typehandlers.add_type_alias(u'void ( * ) ( ns3::Time )*', u'ns3::Time::TracedCallback*')
    typehandlers.add_type_alias(u'void ( * ) ( ns3::Time )&', u'ns3::Time::TracedCallback&')
    ## nstime.h (module 'core'): ns3::Time [class]
    root_module['ns3::Time'].implicitly_converts_to(root_module['ns3::int64x64_t'])
    ## trace-source-accessor.h (module 'core'): ns3::TraceSourceAccessor [class]
    module.add_class('TraceSourceAccessor', import_from_module='ns.core', parent=root_module['ns3::SimpleRefCount< ns3::TraceSourceAccessor, ns3::empty, ns3::DefaultDeleter<ns3::TraceSourceAccessor> >'])
    ## trailer.h (module 'network'): ns3::Trailer [class]
    module.add_class('Trailer', import_from_module='ns.network', parent=root_module['ns3::Chunk'])
    ## random-variable-stream.h (module 'core'): ns3::TriangularRandomVariable [class]
    module.add_class('TriangularRandomVariable', import_from_module='ns.core', parent=root_module['ns3::RandomVariableStream'])
    ## propagation-loss-model.h (module 'propagation'): ns3::TwoRayGroundPropagationLossModel [class]
    module.add_class('TwoRayGroundPropagationLossModel', import_from_module='ns.propagation', parent=root_module['ns3::PropagationLossModel'])
    ## lte-enb-rrc.h (module 'lte'): ns3::UeManager [class]
    module.add_class('UeManager', parent=root_module['ns3::Object'])
    ## lte-enb-rrc.h (module 'lte'): ns3::UeManager::State [enumeration]
    module.add_enum('State', ['INITIAL_RANDOM_ACCESS', 'CONNECTION_SETUP', 'CONNECTION_REJECTED', 'ATTACH_REQUEST', 'CONNECTED_NORMALLY', 'CONNECTION_RECONFIGURATION', 'CONNECTION_REESTABLISHMENT', 'HANDOVER_PREPARATION', 'HANDOVER_JOINING', 'HANDOVER_PATH_SWITCH', 'HANDOVER_LEAVING', 'NUM_STATES'], outer_class=root_module['ns3::UeManager'])
    typehandlers.add_type_alias(u'void ( * ) ( uint64_t const, uint16_t const, uint16_t const, ns3::UeManager::State const, ns3::UeManager::State const )', u'ns3::UeManager::StateTracedCallback')
    typehandlers.add_type_alias(u'void ( * ) ( uint64_t const, uint16_t const, uint16_t const, ns3::UeManager::State const, ns3::UeManager::State const )*', u'ns3::UeManager::StateTracedCallback*')
    typehandlers.add_type_alias(u'void ( * ) ( uint64_t const, uint16_t const, uint16_t const, ns3::UeManager::State const, ns3::UeManager::State const )&', u'ns3::UeManager::StateTracedCallback&')
    ## random-variable-stream.h (module 'core'): ns3::UniformRandomVariable [class]
    module.add_class('UniformRandomVariable', import_from_module='ns.core', parent=root_module['ns3::RandomVariableStream'])
    ## ff-mac-common.h (module 'lte'): ns3::VendorSpecificValue [struct]
    module.add_class('VendorSpecificValue', parent=root_module['ns3::SimpleRefCount< ns3::VendorSpecificValue, ns3::empty, ns3::DefaultDeleter<ns3::VendorSpecificValue> >'])
    ## random-variable-stream.h (module 'core'): ns3::WeibullRandomVariable [class]
    module.add_class('WeibullRandomVariable', import_from_module='ns.core', parent=root_module['ns3::RandomVariableStream'])
    ## epc-x2.h (module 'lte'): ns3::X2CellInfo [class]
    module.add_class('X2CellInfo', parent=root_module['ns3::SimpleRefCount< ns3::X2CellInfo, ns3::empty, ns3::DefaultDeleter<ns3::X2CellInfo> >'])
    ## epc-x2.h (module 'lte'): ns3::X2IfaceInfo [class]
    module.add_class('X2IfaceInfo', parent=root_module['ns3::SimpleRefCount< ns3::X2IfaceInfo, ns3::empty, ns3::DefaultDeleter<ns3::X2IfaceInfo> >'])
    ## random-variable-stream.h (module 'core'): ns3::ZetaRandomVariable [class]
    module.add_class('ZetaRandomVariable', import_from_module='ns.core', parent=root_module['ns3::RandomVariableStream'])
    ## random-variable-stream.h (module 'core'): ns3::ZipfRandomVariable [class]
    module.add_class('ZipfRandomVariable', import_from_module='ns.core', parent=root_module['ns3::RandomVariableStream'])
    ## application.h (module 'network'): ns3::Application [class]
    module.add_class('Application', import_from_module='ns.network', parent=root_module['ns3::Object'])
    typehandlers.add_type_alias(u'void ( * ) ( ns3::Time const &, ns3::Address const & )', u'ns3::Application::DelayAddressCallback')
    typehandlers.add_type_alias(u'void ( * ) ( ns3::Time const &, ns3::Address const & )*', u'ns3::Application::DelayAddressCallback*')
    typehandlers.add_type_alias(u'void ( * ) ( ns3::Time const &, ns3::Address const & )&', u'ns3::Application::DelayAddressCallback&')
    typehandlers.add_type_alias(u'void ( * ) ( std::string const &, std::string const & )', u'ns3::Application::StateTransitionCallback')
    typehandlers.add_type_alias(u'void ( * ) ( std::string const &, std::string const & )*', u'ns3::Application::StateTransitionCallback*')
    typehandlers.add_type_alias(u'void ( * ) ( std::string const &, std::string const & )&', u'ns3::Application::StateTransitionCallback&')
    ## lte-asn1-header.h (module 'lte'): ns3::Asn1Header [class]
    module.add_class('Asn1Header', parent=root_module['ns3::Header'])
    ## attribute.h (module 'core'): ns3::AttributeAccessor [class]
    module.add_class('AttributeAccessor', import_from_module='ns.core', parent=root_module['ns3::SimpleRefCount< ns3::AttributeAccessor, ns3::empty, ns3::DefaultDeleter<ns3::AttributeAccessor> >'])
    ## attribute.h (module 'core'): ns3::AttributeChecker [class]
    module.add_class('AttributeChecker', allow_subclassing=False, automatic_type_narrowing=True, import_from_module='ns.core', parent=root_module['ns3::SimpleRefCount< ns3::AttributeChecker, ns3::empty, ns3::DefaultDeleter<ns3::AttributeChecker> >'])
    ## attribute.h (module 'core'): ns3::AttributeValue [class]
    module.add_class('AttributeValue', allow_subclassing=False, automatic_type_narrowing=True, import_from_module='ns.core', parent=root_module['ns3::SimpleRefCount< ns3::AttributeValue, ns3::empty, ns3::DefaultDeleter<ns3::AttributeValue> >'])
    ## boolean.h (module 'core'): ns3::BooleanChecker [class]
    module.add_class('BooleanChecker', import_from_module='ns.core', parent=root_module['ns3::AttributeChecker'])
    ## boolean.h (module 'core'): ns3::BooleanValue [class]
    module.add_class('BooleanValue', import_from_module='ns.core', parent=root_module['ns3::AttributeValue'])
    ## callback.h (module 'core'): ns3::CallbackChecker [class]
    module.add_class('CallbackChecker', import_from_module='ns.core', parent=root_module['ns3::AttributeChecker'])
    ## callback.h (module 'core'): ns3::CallbackImplBase [class]
    module.add_class('CallbackImplBase', import_from_module='ns.core', parent=root_module['ns3::SimpleRefCount< ns3::CallbackImplBase, ns3::empty, ns3::DefaultDeleter<ns3::CallbackImplBase> >'])
    ## callback.h (module 'core'): ns3::CallbackValue [class]
    module.add_class('CallbackValue', import_from_module='ns.core', parent=root_module['ns3::AttributeValue'])
    ## cc-helper.h (module 'lte'): ns3::CcHelper [class]
    module.add_class('CcHelper', parent=root_module['ns3::Object'])
    ## channel.h (module 'network'): ns3::Channel [class]
    module.add_class('Channel', import_from_module='ns.network', parent=root_module['ns3::Object'])
    ## component-carrier.h (module 'lte'): ns3::ComponentCarrier [class]
    module.add_class('ComponentCarrier', parent=root_module['ns3::Object'])
    ## component-carrier.h (module 'lte'): ns3::ComponentCarrierBaseStation [class]
    module.add_class('ComponentCarrierBaseStation', parent=root_module['ns3::ComponentCarrier'])
    ## component-carrier-enb.h (module 'lte'): ns3::ComponentCarrierEnb [class]
    module.add_class('ComponentCarrierEnb', parent=root_module['ns3::ComponentCarrierBaseStation'])
    ## component-carrier-ue.h (module 'lte'): ns3::ComponentCarrierUe [class]
    module.add_class('ComponentCarrierUe', parent=root_module['ns3::ComponentCarrier'])
    ## random-variable-stream.h (module 'core'): ns3::ConstantRandomVariable [class]
    module.add_class('ConstantRandomVariable', import_from_module='ns.core', parent=root_module['ns3::RandomVariableStream'])
    ## propagation-delay-model.h (module 'propagation'): ns3::ConstantSpeedPropagationDelayModel [class]
    module.add_class('ConstantSpeedPropagationDelayModel', import_from_module='ns.propagation', parent=root_module['ns3::PropagationDelayModel'])
    ## data-calculator.h (module 'stats'): ns3::DataCalculator [class]
    module.add_class('DataCalculator', import_from_module='ns.stats', parent=root_module['ns3::Object'])
    ## data-output-interface.h (module 'stats'): ns3::DataOutputInterface [class]
    module.add_class('DataOutputInterface', import_from_module='ns.stats', parent=root_module['ns3::Object'])
    ## data-rate.h (module 'network'): ns3::DataRateChecker [class]
    module.add_class('DataRateChecker', import_from_module='ns.network', parent=root_module['ns3::AttributeChecker'])
    ## data-rate.h (module 'network'): ns3::DataRateValue [class]
    module.add_class('DataRateValue', import_from_module='ns.network', parent=root_module['ns3::AttributeValue'])
    ## random-variable-stream.h (module 'core'): ns3::DeterministicRandomVariable [class]
    module.add_class('DeterministicRandomVariable', import_from_module='ns.core', parent=root_module['ns3::RandomVariableStream'])
    ## double.h (module 'core'): ns3::DoubleValue [class]
    module.add_class('DoubleValue', import_from_module='ns.core', parent=root_module['ns3::AttributeValue'])
    ## random-variable-stream.h (module 'core'): ns3::EmpiricalRandomVariable [class]
    module.add_class('EmpiricalRandomVariable', import_from_module='ns.core', parent=root_module['ns3::RandomVariableStream'])
    ## attribute.h (module 'core'): ns3::EmptyAttributeAccessor [class]
    module.add_class('EmptyAttributeAccessor', import_from_module='ns.core', parent=root_module['ns3::AttributeAccessor'])
    ## attribute.h (module 'core'): ns3::EmptyAttributeChecker [class]
    module.add_class('EmptyAttributeChecker', import_from_module='ns.core', parent=root_module['ns3::AttributeChecker'])
    ## attribute.h (module 'core'): ns3::EmptyAttributeValue [class]
    module.add_class('EmptyAttributeValue', import_from_module='ns.core', parent=root_module['ns3::AttributeValue'])
    ## enum.h (module 'core'): ns3::EnumChecker [class]
    module.add_class('EnumChecker', import_from_module='ns.core', parent=root_module['ns3::AttributeChecker'])
    ## enum.h (module 'core'): ns3::EnumValue [class]
    module.add_class('EnumValue', import_from_module='ns.core', parent=root_module['ns3::AttributeValue'])
    ## epc-enb-application.h (module 'lte'): ns3::EpcEnbApplication [class]
    module.add_class('EpcEnbApplication', parent=root_module['ns3::Application'])
    ## epc-enb-application.h (module 'lte'): ns3::EpcEnbApplication::EpsFlowId_t [struct]
    module.add_class('EpsFlowId_t', outer_class=root_module['ns3::EpcEnbApplication'])
    typehandlers.add_type_alias(u'void ( * ) ( ns3::Ptr< ns3::Packet > )', u'ns3::EpcEnbApplication::RxTracedCallback')
    typehandlers.add_type_alias(u'void ( * ) ( ns3::Ptr< ns3::Packet > )*', u'ns3::EpcEnbApplication::RxTracedCallback*')
    typehandlers.add_type_alias(u'void ( * ) ( ns3::Ptr< ns3::Packet > )&', u'ns3::EpcEnbApplication::RxTracedCallback&')
    ## epc-helper.h (module 'lte'): ns3::EpcHelper [class]
    module.add_class('EpcHelper', parent=root_module['ns3::Object'])
    ## epc-mme-application.h (module 'lte'): ns3::EpcMmeApplication [class]
    module.add_class('EpcMmeApplication', parent=root_module['ns3::Application'])
    ## epc-pgw-application.h (module 'lte'): ns3::EpcPgwApplication [class]
    module.add_class('EpcPgwApplication', parent=root_module['ns3::Application'])
    typehandlers.add_type_alias(u'void ( * ) ( ns3::Ptr< ns3::Packet > )', u'ns3::EpcPgwApplication::RxTracedCallback')
    typehandlers.add_type_alias(u'void ( * ) ( ns3::Ptr< ns3::Packet > )*', u'ns3::EpcPgwApplication::RxTracedCallback*')
    typehandlers.add_type_alias(u'void ( * ) ( ns3::Ptr< ns3::Packet > )&', u'ns3::EpcPgwApplication::RxTracedCallback&')
    ## epc-sgw-application.h (module 'lte'): ns3::EpcSgwApplication [class]
    module.add_class('EpcSgwApplication', parent=root_module['ns3::Application'])
    ## epc-tft.h (module 'lte'): ns3::EpcTft [class]
    module.add_class('EpcTft', parent=root_module['ns3::SimpleRefCount< ns3::EpcTft, ns3::empty, ns3::DefaultDeleter<ns3::EpcTft> >'])
    ## epc-tft.h (module 'lte'): ns3::EpcTft::Direction [enumeration]
    module.add_enum('Direction', ['DOWNLINK', 'UPLINK', 'BIDIRECTIONAL'], outer_class=root_module['ns3::EpcTft'])
    ## epc-tft.h (module 'lte'): ns3::EpcTft::PacketFilter [struct]
    module.add_class('PacketFilter', outer_class=root_module['ns3::EpcTft'])
    ## epc-tft-classifier.h (module 'lte'): ns3::EpcTftClassifier [class]
    module.add_class('EpcTftClassifier', parent=root_module['ns3::SimpleRefCount< ns3::EpcTftClassifier, ns3::empty, ns3::DefaultDeleter<ns3::EpcTftClassifier> >'])
    ## epc-ue-nas.h (module 'lte'): ns3::EpcUeNas [class]
    module.add_class('EpcUeNas', parent=root_module['ns3::Object'])
    ## epc-ue-nas.h (module 'lte'): ns3::EpcUeNas::State [enumeration]
    module.add_enum('State', ['OFF', 'ATTACHING', 'IDLE_REGISTERED', 'CONNECTING_TO_EPC', 'ACTIVE', 'NUM_STATES'], outer_class=root_module['ns3::EpcUeNas'])
    typehandlers.add_type_alias(u'void ( * ) ( ns3::EpcUeNas::State const, ns3::EpcUeNas::State const )', u'ns3::EpcUeNas::StateTracedCallback')
    typehandlers.add_type_alias(u'void ( * ) ( ns3::EpcUeNas::State const, ns3::EpcUeNas::State const )*', u'ns3::EpcUeNas::StateTracedCallback*')
    typehandlers.add_type_alias(u'void ( * ) ( ns3::EpcUeNas::State const, ns3::EpcUeNas::State const )&', u'ns3::EpcUeNas::StateTracedCallback&')
    ## epc-x2.h (module 'lte'): ns3::EpcX2 [class]
    module.add_class('EpcX2', parent=root_module['ns3::Object'])
    ## epc-x2-header.h (module 'lte'): ns3::EpcX2HandoverPreparationFailureHeader [class]
    module.add_class('EpcX2HandoverPreparationFailureHeader', parent=root_module['ns3::Header'])
    ## epc-x2-header.h (module 'lte'): ns3::EpcX2HandoverRequestAckHeader [class]
    module.add_class('EpcX2HandoverRequestAckHeader', parent=root_module['ns3::Header'])
    ## epc-x2-header.h (module 'lte'): ns3::EpcX2HandoverRequestHeader [class]
    module.add_class('EpcX2HandoverRequestHeader', parent=root_module['ns3::Header'])
    ## epc-x2-header.h (module 'lte'): ns3::EpcX2Header [class]
    module.add_class('EpcX2Header', parent=root_module['ns3::Header'])
    ## epc-x2-header.h (module 'lte'): ns3::EpcX2Header::ProcedureCode_t [enumeration]
    module.add_enum('ProcedureCode_t', ['HandoverPreparation', 'LoadIndication', 'SnStatusTransfer', 'UeContextRelease', 'ResourceStatusReporting'], outer_class=root_module['ns3::EpcX2Header'])
    ## epc-x2-header.h (module 'lte'): ns3::EpcX2Header::TypeOfMessage_t [enumeration]
    module.add_enum('TypeOfMessage_t', ['InitiatingMessage', 'SuccessfulOutcome', 'UnsuccessfulOutcome'], outer_class=root_module['ns3::EpcX2Header'])
    ## epc-x2-header.h (module 'lte'): ns3::EpcX2LoadInformationHeader [class]
    module.add_class('EpcX2LoadInformationHeader', parent=root_module['ns3::Header'])
    ## epc-x2-header.h (module 'lte'): ns3::EpcX2ResourceStatusUpdateHeader [class]
    module.add_class('EpcX2ResourceStatusUpdateHeader', parent=root_module['ns3::Header'])
    ## epc-x2-header.h (module 'lte'): ns3::EpcX2SnStatusTransferHeader [class]
    module.add_class('EpcX2SnStatusTransferHeader', parent=root_module['ns3::Header'])
    ## epc-x2-header.h (module 'lte'): ns3::EpcX2UeContextReleaseHeader [class]
    module.add_class('EpcX2UeContextReleaseHeader', parent=root_module['ns3::Header'])
    ## random-variable-stream.h (module 'core'): ns3::ErlangRandomVariable [class]
    module.add_class('ErlangRandomVariable', import_from_module='ns.core', parent=root_module['ns3::RandomVariableStream'])
    ## event-impl.h (module 'core'): ns3::EventImpl [class]
    module.add_class('EventImpl', import_from_module='ns.core', parent=root_module['ns3::SimpleRefCount< ns3::EventImpl, ns3::empty, ns3::DefaultDeleter<ns3::EventImpl> >'])
    ## random-variable-stream.h (module 'core'): ns3::ExponentialRandomVariable [class]
    module.add_class('ExponentialRandomVariable', import_from_module='ns.core', parent=root_module['ns3::RandomVariableStream'])
    ## ff-mac-scheduler.h (module 'lte'): ns3::FfMacScheduler [class]
    module.add_class('FfMacScheduler', parent=root_module['ns3::Object'])
    ## ff-mac-scheduler.h (module 'lte'): ns3::FfMacScheduler::UlCqiFilter_t [enumeration]
    module.add_enum('UlCqiFilter_t', ['SRS_UL_CQI', 'PUSCH_UL_CQI'], outer_class=root_module['ns3::FfMacScheduler'])
    ## propagation-loss-model.h (module 'propagation'): ns3::FixedRssLossModel [class]
    module.add_class('FixedRssLossModel', import_from_module='ns.propagation', parent=root_module['ns3::PropagationLossModel'])
    ## propagation-loss-model.h (module 'propagation'): ns3::FriisPropagationLossModel [class]
    module.add_class('FriisPropagationLossModel', import_from_module='ns.propagation', parent=root_module['ns3::PropagationLossModel'])
    ## random-variable-stream.h (module 'core'): ns3::GammaRandomVariable [class]
    module.add_class('GammaRandomVariable', import_from_module='ns.core', parent=root_module['ns3::RandomVariableStream'])
    ## epc-gtpc-header.h (module 'lte'): ns3::GtpcHeader [class]
    module.add_class('GtpcHeader', parent=root_module['ns3::Header'])
    ## epc-gtpc-header.h (module 'lte'): ns3::GtpcHeader::InterfaceType_t [enumeration]
    module.add_enum('InterfaceType_t', ['S1U_ENB_GTPU', 'S5_SGW_GTPU', 'S5_PGW_GTPU', 'S5_SGW_GTPC', 'S5_PGW_GTPC', 'S11_MME_GTPC'], outer_class=root_module['ns3::GtpcHeader'])
    ## epc-gtpc-header.h (module 'lte'): ns3::GtpcHeader::MessageType_t [enumeration]
    module.add_enum('MessageType_t', ['Reserved', 'CreateSessionRequest', 'CreateSessionResponse', 'ModifyBearerRequest', 'ModifyBearerResponse', 'DeleteSessionRequest', 'DeleteSessionResponse', 'DeleteBearerCommand', 'DeleteBearerRequest', 'DeleteBearerResponse'], outer_class=root_module['ns3::GtpcHeader'])
    ## epc-gtpc-header.h (module 'lte'): ns3::GtpcHeader::Fteid_t [struct]
    module.add_class('Fteid_t', outer_class=root_module['ns3::GtpcHeader'])
    ## epc-gtpc-header.h (module 'lte'): ns3::GtpcModifyBearerRequestMessage [class]
    module.add_class('GtpcModifyBearerRequestMessage', parent=[root_module['ns3::GtpcHeader'], root_module['ns3::GtpcIes']])
    ## epc-gtpc-header.h (module 'lte'): ns3::GtpcModifyBearerRequestMessage::BearerContextToBeModified [struct]
    module.add_class('BearerContextToBeModified', outer_class=root_module['ns3::GtpcModifyBearerRequestMessage'])
    ## epc-gtpc-header.h (module 'lte'): ns3::GtpcModifyBearerResponseMessage [class]
    module.add_class('GtpcModifyBearerResponseMessage', parent=[root_module['ns3::GtpcHeader'], root_module['ns3::GtpcIes']])
    ## epc-gtpu-header.h (module 'lte'): ns3::GtpuHeader [class]
    module.add_class('GtpuHeader', parent=root_module['ns3::Header'])
    ## integer.h (module 'core'): ns3::IntegerValue [class]
    module.add_class('IntegerValue', import_from_module='ns.core', parent=root_module['ns3::AttributeValue'])
    ## ipv4.h (module 'internet'): ns3::Ipv4 [class]
    module.add_class('Ipv4', import_from_module='ns.internet', parent=root_module['ns3::Object'])
    ## ipv4-address.h (module 'network'): ns3::Ipv4AddressChecker [class]
    module.add_class('Ipv4AddressChecker', import_from_module='ns.network', parent=root_module['ns3::AttributeChecker'])
    ## ipv4-address.h (module 'network'): ns3::Ipv4AddressValue [class]
    module.add_class('Ipv4AddressValue', import_from_module='ns.network', parent=root_module['ns3::AttributeValue'])
    ## ipv4-address.h (module 'network'): ns3::Ipv4MaskChecker [class]
    module.add_class('Ipv4MaskChecker', import_from_module='ns.network', parent=root_module['ns3::AttributeChecker'])
    ## ipv4-address.h (module 'network'): ns3::Ipv4MaskValue [class]
    module.add_class('Ipv4MaskValue', import_from_module='ns.network', parent=root_module['ns3::AttributeValue'])
    ## ipv4-route.h (module 'internet'): ns3::Ipv4MulticastRoute [class]
    module.add_class('Ipv4MulticastRoute', import_from_module='ns.internet', parent=root_module['ns3::SimpleRefCount< ns3::Ipv4MulticastRoute, ns3::empty, ns3::DefaultDeleter<ns3::Ipv4MulticastRoute> >'])
    ## ipv4-route.h (module 'internet'): ns3::Ipv4Route [class]
    module.add_class('Ipv4Route', import_from_module='ns.internet', parent=root_module['ns3::SimpleRefCount< ns3::Ipv4Route, ns3::empty, ns3::DefaultDeleter<ns3::Ipv4Route> >'])
    ## ipv6.h (module 'internet'): ns3::Ipv6 [class]
    module.add_class('Ipv6', import_from_module='ns.internet', parent=root_module['ns3::Object'])
    ## ipv6-address.h (module 'network'): ns3::Ipv6AddressChecker [class]
    module.add_class('Ipv6AddressChecker', import_from_module='ns.network', parent=root_module['ns3::AttributeChecker'])
    ## ipv6-address.h (module 'network'): ns3::Ipv6AddressValue [class]
    module.add_class('Ipv6AddressValue', import_from_module='ns.network', parent=root_module['ns3::AttributeValue'])
    ## ipv6-address.h (module 'network'): ns3::Ipv6PrefixChecker [class]
    module.add_class('Ipv6PrefixChecker', import_from_module='ns.network', parent=root_module['ns3::AttributeChecker'])
    ## ipv6-address.h (module 'network'): ns3::Ipv6PrefixValue [class]
    module.add_class('Ipv6PrefixValue', import_from_module='ns.network', parent=root_module['ns3::AttributeValue'])
    ## propagation-loss-model.h (module 'propagation'): ns3::LogDistancePropagationLossModel [class]
    module.add_class('LogDistancePropagationLossModel', import_from_module='ns.propagation', parent=root_module['ns3::PropagationLossModel'])
    ## random-variable-stream.h (module 'core'): ns3::LogNormalRandomVariable [class]
    module.add_class('LogNormalRandomVariable', import_from_module='ns.core', parent=root_module['ns3::RandomVariableStream'])
    ## lte-3gpp-hex-grid-enb-topology-helper.h (module 'lte'): ns3::Lte3gppHexGridEnbTopologyHelper [class]
    module.add_class('Lte3gppHexGridEnbTopologyHelper', parent=root_module['ns3::Object'])
    ## lte-amc.h (module 'lte'): ns3::LteAmc [class]
    module.add_class('LteAmc', parent=root_module['ns3::Object'])
    ## lte-amc.h (module 'lte'): ns3::LteAmc::AmcModel [enumeration]
    module.add_enum('AmcModel', ['PiroEW2010', 'MiErrorModel', 'NistErrorModel'], outer_class=root_module['ns3::LteAmc'])
    ## lte-anr.h (module 'lte'): ns3::LteAnr [class]
    module.add_class('LteAnr', parent=root_module['ns3::Object'])
    ## lte-chunk-processor.h (module 'lte'): ns3::LteChunkProcessor [class]
    module.add_class('LteChunkProcessor', parent=root_module['ns3::SimpleRefCount< ns3::LteChunkProcessor, ns3::empty, ns3::DefaultDeleter<ns3::LteChunkProcessor> >'])
    ## lte-control-messages.h (module 'lte'): ns3::LteControlMessage [class]
    module.add_class('LteControlMessage', parent=root_module['ns3::SimpleRefCount< ns3::LteControlMessage, ns3::empty, ns3::DefaultDeleter<ns3::LteControlMessage> >'])
    ## lte-control-messages.h (module 'lte'): ns3::LteControlMessage::MessageType [enumeration]
    module.add_enum('MessageType', ['DL_DCI', 'UL_DCI', 'DL_CQI', 'UL_CQI', 'BSR', 'DL_HARQ', 'RACH_PREAMBLE', 'RAR', 'MIB', 'SIB1', 'SL_DCI'], outer_class=root_module['ns3::LteControlMessage'])
    ## lte-enb-component-carrier-manager.h (module 'lte'): ns3::LteEnbComponentCarrierManager [class]
    module.add_class('LteEnbComponentCarrierManager', parent=root_module['ns3::Object'])
    ## lte-enb-mac.h (module 'lte'): ns3::LteEnbMac [class]
    module.add_class('LteEnbMac', parent=root_module['ns3::Object'])
    typehandlers.add_type_alias(u'void ( * ) ( uint32_t const, uint32_t const, uint16_t const, uint8_t const, uint16_t const, uint8_t const, uint16_t const, uint8_t const )', u'ns3::LteEnbMac::DlSchedulingTracedCallback')
    typehandlers.add_type_alias(u'void ( * ) ( uint32_t const, uint32_t const, uint16_t const, uint8_t const, uint16_t const, uint8_t const, uint16_t const, uint8_t const )*', u'ns3::LteEnbMac::DlSchedulingTracedCallback*')
    typehandlers.add_type_alias(u'void ( * ) ( uint32_t const, uint32_t const, uint16_t const, uint8_t const, uint16_t const, uint8_t const, uint16_t const, uint8_t const )&', u'ns3::LteEnbMac::DlSchedulingTracedCallback&')
    typehandlers.add_type_alias(u'void ( * ) ( uint32_t const, uint32_t const, uint16_t const, uint8_t const, uint16_t const )', u'ns3::LteEnbMac::UlSchedulingTracedCallback')
    typehandlers.add_type_alias(u'void ( * ) ( uint32_t const, uint32_t const, uint16_t const, uint8_t const, uint16_t const )*', u'ns3::LteEnbMac::UlSchedulingTracedCallback*')
    typehandlers.add_type_alias(u'void ( * ) ( uint32_t const, uint32_t const, uint16_t const, uint8_t const, uint16_t const )&', u'ns3::LteEnbMac::UlSchedulingTracedCallback&')
    ## lte-enb-rrc.h (module 'lte'): ns3::LteEnbRrc [class]
    module.add_class('LteEnbRrc', parent=root_module['ns3::Object'])
    ## lte-enb-rrc.h (module 'lte'): ns3::LteEnbRrc::LteEpsBearerToRlcMapping_t [enumeration]
    module.add_enum('LteEpsBearerToRlcMapping_t', ['RLC_SM_ALWAYS', 'RLC_UM_ALWAYS', 'RLC_AM_ALWAYS', 'PER_BASED'], outer_class=root_module['ns3::LteEnbRrc'])
    typehandlers.add_type_alias(u'void ( * ) ( uint16_t const, uint16_t const )', u'ns3::LteEnbRrc::NewUeContextTracedCallback')
    typehandlers.add_type_alias(u'void ( * ) ( uint16_t const, uint16_t const )*', u'ns3::LteEnbRrc::NewUeContextTracedCallback*')
    typehandlers.add_type_alias(u'void ( * ) ( uint16_t const, uint16_t const )&', u'ns3::LteEnbRrc::NewUeContextTracedCallback&')
    typehandlers.add_type_alias(u'void ( * ) ( uint64_t const, uint16_t const, uint16_t const )', u'ns3::LteEnbRrc::ConnectionHandoverTracedCallback')
    typehandlers.add_type_alias(u'void ( * ) ( uint64_t const, uint16_t const, uint16_t const )*', u'ns3::LteEnbRrc::ConnectionHandoverTracedCallback*')
    typehandlers.add_type_alias(u'void ( * ) ( uint64_t const, uint16_t const, uint16_t const )&', u'ns3::LteEnbRrc::ConnectionHandoverTracedCallback&')
    typehandlers.add_type_alias(u'void ( * ) ( uint64_t const, uint16_t const, uint16_t const, uint16_t const )', u'ns3::LteEnbRrc::HandoverStartTracedCallback')
    typehandlers.add_type_alias(u'void ( * ) ( uint64_t const, uint16_t const, uint16_t const, uint16_t const )*', u'ns3::LteEnbRrc::HandoverStartTracedCallback*')
    typehandlers.add_type_alias(u'void ( * ) ( uint64_t const, uint16_t const, uint16_t const, uint16_t const )&', u'ns3::LteEnbRrc::HandoverStartTracedCallback&')
    typehandlers.add_type_alias(u'void ( * ) ( uint64_t const, uint16_t const, uint16_t const, ns3::LteRrcSap::MeasurementReport const )', u'ns3::LteEnbRrc::ReceiveReportTracedCallback')
    typehandlers.add_type_alias(u'void ( * ) ( uint64_t const, uint16_t const, uint16_t const, ns3::LteRrcSap::MeasurementReport const )*', u'ns3::LteEnbRrc::ReceiveReportTracedCallback*')
    typehandlers.add_type_alias(u'void ( * ) ( uint64_t const, uint16_t const, uint16_t const, ns3::LteRrcSap::MeasurementReport const )&', u'ns3::LteEnbRrc::ReceiveReportTracedCallback&')
    typehandlers.add_type_alias(u'void ( * ) ( uint64_t const, uint16_t const, uint16_t const, std::string const )', u'ns3::LteEnbRrc::TimerExpiryTracedCallback')
    typehandlers.add_type_alias(u'void ( * ) ( uint64_t const, uint16_t const, uint16_t const, std::string const )*', u'ns3::LteEnbRrc::TimerExpiryTracedCallback*')
    typehandlers.add_type_alias(u'void ( * ) ( uint64_t const, uint16_t const, uint16_t const, std::string const )&', u'ns3::LteEnbRrc::TimerExpiryTracedCallback&')
    ## lte-rrc-protocol-ideal.h (module 'lte'): ns3::LteEnbRrcProtocolIdeal [class]
    module.add_class('LteEnbRrcProtocolIdeal', parent=root_module['ns3::Object'])
    ## lte-rrc-protocol-real.h (module 'lte'): ns3::LteEnbRrcProtocolReal [class]
    module.add_class('LteEnbRrcProtocolReal', parent=root_module['ns3::Object'])
    ## lte-ffr-algorithm.h (module 'lte'): ns3::LteFfrAlgorithm [class]
    module.add_class('LteFfrAlgorithm', parent=root_module['ns3::Object'])
    ## lte-ffr-distributed-algorithm.h (module 'lte'): ns3::LteFfrDistributedAlgorithm [class]
    module.add_class('LteFfrDistributedAlgorithm', parent=root_module['ns3::LteFfrAlgorithm'])
    ## lte-ffr-enhanced-algorithm.h (module 'lte'): ns3::LteFfrEnhancedAlgorithm [class]
    module.add_class('LteFfrEnhancedAlgorithm', parent=root_module['ns3::LteFfrAlgorithm'])
    ## lte-ffr-soft-algorithm.h (module 'lte'): ns3::LteFfrSoftAlgorithm [class]
    module.add_class('LteFfrSoftAlgorithm', parent=root_module['ns3::LteFfrAlgorithm'])
    ## lte-fr-hard-algorithm.h (module 'lte'): ns3::LteFrHardAlgorithm [class]
    module.add_class('LteFrHardAlgorithm', parent=root_module['ns3::LteFfrAlgorithm'])
    ## lte-fr-no-op-algorithm.h (module 'lte'): ns3::LteFrNoOpAlgorithm [class]
    module.add_class('LteFrNoOpAlgorithm', parent=root_module['ns3::LteFfrAlgorithm'])
    ## lte-fr-soft-algorithm.h (module 'lte'): ns3::LteFrSoftAlgorithm [class]
    module.add_class('LteFrSoftAlgorithm', parent=root_module['ns3::LteFfrAlgorithm'])
    ## lte-fr-strict-algorithm.h (module 'lte'): ns3::LteFrStrictAlgorithm [class]
    module.add_class('LteFrStrictAlgorithm', parent=root_module['ns3::LteFfrAlgorithm'])
    ## lte-handover-algorithm.h (module 'lte'): ns3::LteHandoverAlgorithm [class]
    module.add_class('LteHandoverAlgorithm', parent=root_module['ns3::Object'])
    ## lte-harq-phy.h (module 'lte'): ns3::LteHarqPhy [class]
    module.add_class('LteHarqPhy', parent=root_module['ns3::SimpleRefCount< ns3::LteHarqPhy, ns3::empty, ns3::DefaultDeleter<ns3::LteHarqPhy> >'])
    ## lte-helper.h (module 'lte'): ns3::LteHelper [class]
    module.add_class('LteHelper', parent=root_module['ns3::Object'])
    ## lte-hex-grid-enb-topology-helper.h (module 'lte'): ns3::LteHexGridEnbTopologyHelper [class]
    module.add_class('LteHexGridEnbTopologyHelper', parent=root_module['ns3::Object'])
    ## lte-interference.h (module 'lte'): ns3::LteInterference [class]
    module.add_class('LteInterference', parent=root_module['ns3::Object'])
    ## lte-pdcp.h (module 'lte'): ns3::LtePdcp [class]
    module.add_class('LtePdcp', parent=root_module['ns3::Object'])
    ## lte-pdcp.h (module 'lte'): ns3::LtePdcp::Status [struct]
    module.add_class('Status', outer_class=root_module['ns3::LtePdcp'])
    typehandlers.add_type_alias(u'void ( * ) ( uint16_t, uint8_t, uint32_t )', u'ns3::LtePdcp::PduTxTracedCallback')
    typehandlers.add_type_alias(u'void ( * ) ( uint16_t, uint8_t, uint32_t )*', u'ns3::LtePdcp::PduTxTracedCallback*')
    typehandlers.add_type_alias(u'void ( * ) ( uint16_t, uint8_t, uint32_t )&', u'ns3::LtePdcp::PduTxTracedCallback&')
    typehandlers.add_type_alias(u'void ( * ) ( uint16_t const, uint8_t const, uint32_t const, uint64_t const )', u'ns3::LtePdcp::PduRxTracedCallback')
    typehandlers.add_type_alias(u'void ( * ) ( uint16_t const, uint8_t const, uint32_t const, uint64_t const )*', u'ns3::LtePdcp::PduRxTracedCallback*')
    typehandlers.add_type_alias(u'void ( * ) ( uint16_t const, uint8_t const, uint32_t const, uint64_t const )&', u'ns3::LtePdcp::PduRxTracedCallback&')
    ## lte-phy.h (module 'lte'): ns3::LtePhy [class]
    module.add_class('LtePhy', parent=root_module['ns3::Object'])
    ## lte-radio-bearer-info.h (module 'lte'): ns3::LteRadioBearerInfo [class]
    module.add_class('LteRadioBearerInfo', parent=root_module['ns3::Object'])
    ## lte-rlc.h (module 'lte'): ns3::LteRlc [class]
    module.add_class('LteRlc', parent=root_module['ns3::Object'])
    ## lte-rlc.h (module 'lte'): ns3::LteRlc::ChannelType [enumeration]
    module.add_enum('ChannelType', ['DEFAULT', 'STCH'], outer_class=root_module['ns3::LteRlc'])
    typehandlers.add_type_alias(u'void ( * ) ( uint16_t, uint8_t, uint32_t )', u'ns3::LteRlc::NotifyTxTracedCallback')
    typehandlers.add_type_alias(u'void ( * ) ( uint16_t, uint8_t, uint32_t )*', u'ns3::LteRlc::NotifyTxTracedCallback*')
    typehandlers.add_type_alias(u'void ( * ) ( uint16_t, uint8_t, uint32_t )&', u'ns3::LteRlc::NotifyTxTracedCallback&')
    typehandlers.add_type_alias(u'void ( * ) ( uint16_t, uint8_t, uint32_t, uint64_t )', u'ns3::LteRlc::ReceiveTracedCallback')
    typehandlers.add_type_alias(u'void ( * ) ( uint16_t, uint8_t, uint32_t, uint64_t )*', u'ns3::LteRlc::ReceiveTracedCallback*')
    typehandlers.add_type_alias(u'void ( * ) ( uint16_t, uint8_t, uint32_t, uint64_t )&', u'ns3::LteRlc::ReceiveTracedCallback&')
    ## lte-rlc-am.h (module 'lte'): ns3::LteRlcAm [class]
    module.add_class('LteRlcAm', parent=root_module['ns3::LteRlc'])
    ## lte-rlc.h (module 'lte'): ns3::LteRlcSm [class]
    module.add_class('LteRlcSm', parent=root_module['ns3::LteRlc'])
    ## lte-rlc-tm.h (module 'lte'): ns3::LteRlcTm [class]
    module.add_class('LteRlcTm', parent=root_module['ns3::LteRlc'])
    ## lte-rlc-um.h (module 'lte'): ns3::LteRlcUm [class]
    module.add_class('LteRlcUm', parent=root_module['ns3::LteRlc'])
    ## lte-sidelink-helper.h (module 'lte'): ns3::LteSidelinkHelper [class]
    module.add_class('LteSidelinkHelper', parent=root_module['ns3::Object'])
    ## lte-sidelink-helper.h (module 'lte'): ns3::LteSidelinkHelper::SrsrpMethod_t [enumeration]
    module.add_enum('SrsrpMethod_t', ['SLRSRP_TX_PW', 'SLRSRP_PSBCH'], outer_class=root_module['ns3::LteSidelinkHelper'])
    ## lte-radio-bearer-info.h (module 'lte'): ns3::LteSidelinkRadioBearerInfo [class]
    module.add_class('LteSidelinkRadioBearerInfo', parent=root_module['ns3::LteRadioBearerInfo'])
    ## lte-radio-bearer-info.h (module 'lte'): ns3::LteSignalingRadioBearerInfo [class]
    module.add_class('LteSignalingRadioBearerInfo', parent=root_module['ns3::LteRadioBearerInfo'])
    ## lte-sl-chunk-processor.h (module 'lte'): ns3::LteSlChunkProcessor [class]
    module.add_class('LteSlChunkProcessor', parent=root_module['ns3::SimpleRefCount< ns3::LteSlChunkProcessor, ns3::empty, ns3::DefaultDeleter<ns3::LteSlChunkProcessor> >'])
    ## lte-sl-chunk-processor.h (module 'lte'): ns3::LteSlChunkProcessor::LteSlChunkValue [struct]
    module.add_class('LteSlChunkValue', outer_class=root_module['ns3::LteSlChunkProcessor'])
    ## lte-sl-disc-preconfig-pool-factory.h (module 'lte'): ns3::LteSlDiscPreconfigPoolFactory [class]
    module.add_class('LteSlDiscPreconfigPoolFactory', parent=root_module['ns3::LteSlDiscResourcePoolFactory'])
    ## lte-sl-enb-rrc.h (module 'lte'): ns3::LteSlEnbRrc [class]
    module.add_class('LteSlEnbRrc', parent=root_module['ns3::Object'])
    ## lte-sl-harq-phy.h (module 'lte'): ns3::LteSlHarqPhy [class]
    module.add_class('LteSlHarqPhy', parent=root_module['ns3::SimpleRefCount< ns3::LteSlHarqPhy, ns3::empty, ns3::DefaultDeleter<ns3::LteSlHarqPhy> >'])
    ## lte-sl-interference.h (module 'lte'): ns3::LteSlInterference [class]
    module.add_class('LteSlInterference', parent=root_module['ns3::Object'])
    ## lte-sl-tft.h (module 'lte'): ns3::LteSlTft [class]
    module.add_class('LteSlTft', parent=root_module['ns3::SimpleRefCount< ns3::LteSlTft, ns3::empty, ns3::DefaultDeleter<ns3::LteSlTft> >'])
    ## lte-sl-tft.h (module 'lte'): ns3::LteSlTft::Direction [enumeration]
    module.add_enum('Direction', ['TRANSMIT', 'RECEIVE', 'BIDIRECTIONAL'], outer_class=root_module['ns3::LteSlTft'])
    ## lte-sl-ue-rrc.h (module 'lte'): ns3::LteSlUeRrc [class]
    module.add_class('LteSlUeRrc', parent=root_module['ns3::Object'])
    ## lte-sl-ue-rrc.h (module 'lte'): ns3::LteSlUeRrc::DiscoveryModel [enumeration]
    module.add_enum('DiscoveryModel', ['ModelA', 'ModelB'], outer_class=root_module['ns3::LteSlUeRrc'])
    ## lte-sl-ue-rrc.h (module 'lte'): ns3::LteSlUeRrc::DiscoveryRole [enumeration]
    module.add_enum('DiscoveryRole', ['Discoveree', 'Discovered'], outer_class=root_module['ns3::LteSlUeRrc'])
    ## lte-sl-ue-rrc.h (module 'lte'): ns3::LteSlUeRrc::RelayRole [enumeration]
    module.add_enum('RelayRole', ['RemoteUE', 'RelayUE'], outer_class=root_module['ns3::LteSlUeRrc'])
    ## lte-sl-ue-rrc.h (module 'lte'): ns3::LteSlUeRrc::AppServiceInfo [struct]
    module.add_class('AppServiceInfo', outer_class=root_module['ns3::LteSlUeRrc'])
    ## lte-sl-ue-rrc.h (module 'lte'): ns3::LteSlUeRrc::RelayServiceInfo [struct]
    module.add_class('RelayServiceInfo', outer_class=root_module['ns3::LteSlUeRrc'])
    ## lte-spectrum-phy.h (module 'lte'): ns3::LteSpectrumPhy [class]
    module.add_class('LteSpectrumPhy', parent=root_module['ns3::SpectrumPhy'])
    ## lte-spectrum-phy.h (module 'lte'): ns3::LteSpectrumPhy::State [enumeration]
    module.add_enum('State', ['IDLE', 'TX_DL_CTRL', 'TX_DATA', 'TX_UL_SRS', 'RX_DL_CTRL', 'RX_DATA', 'RX_UL_SRS'], outer_class=root_module['ns3::LteSpectrumPhy'])
    typehandlers.add_type_alias(u'void ( * ) ( uint64_t )', u'ns3::LteSpectrumPhy::DropSlTbTracedCallback')
    typehandlers.add_type_alias(u'void ( * ) ( uint64_t )*', u'ns3::LteSpectrumPhy::DropSlTbTracedCallback*')
    typehandlers.add_type_alias(u'void ( * ) ( uint64_t )&', u'ns3::LteSpectrumPhy::DropSlTbTracedCallback&')
    typehandlers.add_type_alias(u'void ( * ) ( ns3::Ptr< ns3::LteSpectrumPhy > )', u'ns3::LteSpectrumPhy::SlStartRxTracedCallback')
    typehandlers.add_type_alias(u'void ( * ) ( ns3::Ptr< ns3::LteSpectrumPhy > )*', u'ns3::LteSpectrumPhy::SlStartRxTracedCallback*')
    typehandlers.add_type_alias(u'void ( * ) ( ns3::Ptr< ns3::LteSpectrumPhy > )&', u'ns3::LteSpectrumPhy::SlStartRxTracedCallback&')
    ## lte-spectrum-signal-parameters.h (module 'lte'): ns3::LteSpectrumSignalParameters [struct]
    module.add_class('LteSpectrumSignalParameters', parent=root_module['ns3::SpectrumSignalParameters'])
    ## lte-spectrum-signal-parameters.h (module 'lte'): ns3::LteSpectrumSignalParametersDataFrame [struct]
    module.add_class('LteSpectrumSignalParametersDataFrame', parent=root_module['ns3::SpectrumSignalParameters'])
    ## lte-spectrum-signal-parameters.h (module 'lte'): ns3::LteSpectrumSignalParametersDlCtrlFrame [struct]
    module.add_class('LteSpectrumSignalParametersDlCtrlFrame', parent=root_module['ns3::SpectrumSignalParameters'])
    ## lte-spectrum-signal-parameters.h (module 'lte'): ns3::LteSpectrumSignalParametersSlFrame [struct]
    module.add_class('LteSpectrumSignalParametersSlFrame', parent=root_module['ns3::SpectrumSignalParameters'])
    ## lte-spectrum-signal-parameters.h (module 'lte'): ns3::LteSpectrumSignalParametersSlMibFrame [struct]
    module.add_class('LteSpectrumSignalParametersSlMibFrame', parent=root_module['ns3::LteSpectrumSignalParametersSlFrame'])
    ## lte-spectrum-signal-parameters.h (module 'lte'): ns3::LteSpectrumSignalParametersUlSrsFrame [struct]
    module.add_class('LteSpectrumSignalParametersUlSrsFrame', parent=root_module['ns3::SpectrumSignalParameters'])
    ## lte-stats-calculator.h (module 'lte'): ns3::LteStatsCalculator [class]
    module.add_class('LteStatsCalculator', parent=root_module['ns3::Object'])
    ## lte-ue-component-carrier-manager.h (module 'lte'): ns3::LteUeComponentCarrierManager [class]
    module.add_class('LteUeComponentCarrierManager', parent=root_module['ns3::Object'])
    ## lte-ue-mac.h (module 'lte'): ns3::LteUeMac [class]
    module.add_class('LteUeMac', parent=root_module['ns3::Object'])
    typehandlers.add_type_alias(u'void ( * ) ( uint64_t, bool, uint8_t, uint8_t )', u'ns3::LteUeMac::RaResponseTimeoutTracedCallback')
    typehandlers.add_type_alias(u'void ( * ) ( uint64_t, bool, uint8_t, uint8_t )*', u'ns3::LteUeMac::RaResponseTimeoutTracedCallback*')
    typehandlers.add_type_alias(u'void ( * ) ( uint64_t, bool, uint8_t, uint8_t )&', u'ns3::LteUeMac::RaResponseTimeoutTracedCallback&')
    ## lte-ue-phy.h (module 'lte'): ns3::LteUePhy [class]
    module.add_class('LteUePhy', parent=root_module['ns3::LtePhy'])
    ## lte-ue-phy.h (module 'lte'): ns3::LteUePhy::State [enumeration]
    module.add_enum('State', ['CELL_SEARCH', 'SYNCHRONIZED', 'NUM_STATES'], outer_class=root_module['ns3::LteUePhy'])
    typehandlers.add_type_alias(u'void ( * ) ( uint16_t, uint16_t, ns3::LteUePhy::State, ns3::LteUePhy::State )', u'ns3::LteUePhy::StateTracedCallback')
    typehandlers.add_type_alias(u'void ( * ) ( uint16_t, uint16_t, ns3::LteUePhy::State, ns3::LteUePhy::State )*', u'ns3::LteUePhy::StateTracedCallback*')
    typehandlers.add_type_alias(u'void ( * ) ( uint16_t, uint16_t, ns3::LteUePhy::State, ns3::LteUePhy::State )&', u'ns3::LteUePhy::StateTracedCallback&')
    typehandlers.add_type_alias(u'void ( * ) ( uint16_t, uint16_t, double, double, uint8_t )', u'ns3::LteUePhy::RsrpSinrTracedCallback')
    typehandlers.add_type_alias(u'void ( * ) ( uint16_t, uint16_t, double, double, uint8_t )*', u'ns3::LteUePhy::RsrpSinrTracedCallback*')
    typehandlers.add_type_alias(u'void ( * ) ( uint16_t, uint16_t, double, double, uint8_t )&', u'ns3::LteUePhy::RsrpSinrTracedCallback&')
    typehandlers.add_type_alias(u'void ( * ) ( uint16_t, uint16_t, double, double, bool, uint8_t )', u'ns3::LteUePhy::RsrpRsrqTracedCallback')
    typehandlers.add_type_alias(u'void ( * ) ( uint16_t, uint16_t, double, double, bool, uint8_t )*', u'ns3::LteUePhy::RsrpRsrqTracedCallback*')
    typehandlers.add_type_alias(u'void ( * ) ( uint16_t, uint16_t, double, double, bool, uint8_t )&', u'ns3::LteUePhy::RsrpRsrqTracedCallback&')
    ## lte-ue-power-control.h (module 'lte'): ns3::LteUePowerControl [class]
    module.add_class('LteUePowerControl', parent=root_module['ns3::Object'])
    typehandlers.add_type_alias(u'void ( * ) ( uint16_t, uint16_t, double )', u'ns3::LteUePowerControl::TxPowerTracedCallback')
    typehandlers.add_type_alias(u'void ( * ) ( uint16_t, uint16_t, double )*', u'ns3::LteUePowerControl::TxPowerTracedCallback*')
    typehandlers.add_type_alias(u'void ( * ) ( uint16_t, uint16_t, double )&', u'ns3::LteUePowerControl::TxPowerTracedCallback&')
    ## lte-ue-rrc.h (module 'lte'): ns3::LteUeRrc [class]
    module.add_class('LteUeRrc', parent=root_module['ns3::Object'])
    ## lte-ue-rrc.h (module 'lte'): ns3::LteUeRrc::State [enumeration]
    module.add_enum('State', ['IDLE_START', 'IDLE_CELL_SEARCH', 'IDLE_WAIT_MIB_SIB1', 'IDLE_WAIT_MIB', 'IDLE_WAIT_SIB1', 'IDLE_CAMPED_NORMALLY', 'IDLE_WAIT_SIB2', 'IDLE_RANDOM_ACCESS', 'IDLE_CONNECTING', 'CONNECTED_NORMALLY', 'CONNECTED_HANDOVER', 'CONNECTED_PHY_PROBLEM', 'CONNECTED_REESTABLISHING', 'NUM_STATES'], outer_class=root_module['ns3::LteUeRrc'])
    ## lte-ue-rrc.h (module 'lte'): ns3::LteUeRrc::SlChangeOfSyncRefStatParameters [struct]
    module.add_class('SlChangeOfSyncRefStatParameters', outer_class=root_module['ns3::LteUeRrc'])
    typehandlers.add_type_alias(u'void ( * ) ( uint64_t, uint16_t )', u'ns3::LteUeRrc::CellSelectionTracedCallback')
    typehandlers.add_type_alias(u'void ( * ) ( uint64_t, uint16_t )*', u'ns3::LteUeRrc::CellSelectionTracedCallback*')
    typehandlers.add_type_alias(u'void ( * ) ( uint64_t, uint16_t )&', u'ns3::LteUeRrc::CellSelectionTracedCallback&')
    typehandlers.add_type_alias(u'void ( * ) ( uint64_t, uint16_t, uint16_t )', u'ns3::LteUeRrc::ImsiCidRntiTracedCallback')
    typehandlers.add_type_alias(u'void ( * ) ( uint64_t, uint16_t, uint16_t )*', u'ns3::LteUeRrc::ImsiCidRntiTracedCallback*')
    typehandlers.add_type_alias(u'void ( * ) ( uint64_t, uint16_t, uint16_t )&', u'ns3::LteUeRrc::ImsiCidRntiTracedCallback&')
    typehandlers.add_type_alias(u'void ( * ) ( uint64_t, uint16_t, uint16_t, uint16_t )', u'ns3::LteUeRrc::MibSibHandoverTracedCallback')
    typehandlers.add_type_alias(u'void ( * ) ( uint64_t, uint16_t, uint16_t, uint16_t )*', u'ns3::LteUeRrc::MibSibHandoverTracedCallback*')
    typehandlers.add_type_alias(u'void ( * ) ( uint64_t, uint16_t, uint16_t, uint16_t )&', u'ns3::LteUeRrc::MibSibHandoverTracedCallback&')
    typehandlers.add_type_alias(u'void ( * ) ( uint64_t, uint16_t, uint16_t, ns3::LteUeRrc::State, ns3::LteUeRrc::State )', u'ns3::LteUeRrc::StateTracedCallback')
    typehandlers.add_type_alias(u'void ( * ) ( uint64_t, uint16_t, uint16_t, ns3::LteUeRrc::State, ns3::LteUeRrc::State )*', u'ns3::LteUeRrc::StateTracedCallback*')
    typehandlers.add_type_alias(u'void ( * ) ( uint64_t, uint16_t, uint16_t, ns3::LteUeRrc::State, ns3::LteUeRrc::State )&', u'ns3::LteUeRrc::StateTracedCallback&')
    typehandlers.add_type_alias(u'void ( * ) ( ns3::Ptr< ns3::LteUeRrc >, std::list< ns3::LteRrcSap::SCellToAddMod > )', u'ns3::LteUeRrc::SCarrierConfiguredTracedCallback')
    typehandlers.add_type_alias(u'void ( * ) ( ns3::Ptr< ns3::LteUeRrc >, std::list< ns3::LteRrcSap::SCellToAddMod > )*', u'ns3::LteUeRrc::SCarrierConfiguredTracedCallback*')
    typehandlers.add_type_alias(u'void ( * ) ( ns3::Ptr< ns3::LteUeRrc >, std::list< ns3::LteRrcSap::SCellToAddMod > )&', u'ns3::LteUeRrc::SCarrierConfiguredTracedCallback&')
    typehandlers.add_type_alias(u'void ( * ) ( uint64_t, uint16_t, uint16_t, std::string, uint16_t )', u'ns3::LteUeRrc::PhySyncDetectionTracedCallback')
    typehandlers.add_type_alias(u'void ( * ) ( uint64_t, uint16_t, uint16_t, std::string, uint16_t )*', u'ns3::LteUeRrc::PhySyncDetectionTracedCallback*')
    typehandlers.add_type_alias(u'void ( * ) ( uint64_t, uint16_t, uint16_t, std::string, uint16_t )&', u'ns3::LteUeRrc::PhySyncDetectionTracedCallback&')
    typehandlers.add_type_alias(u'void ( * ) ( uint64_t, uint16_t, uint16_t, uint8_t )', u'ns3::LteUeRrc::ImsiCidRntiCountTracedCallback')
    typehandlers.add_type_alias(u'void ( * ) ( uint64_t, uint16_t, uint16_t, uint8_t )*', u'ns3::LteUeRrc::ImsiCidRntiCountTracedCallback*')
    typehandlers.add_type_alias(u'void ( * ) ( uint64_t, uint16_t, uint16_t, uint8_t )&', u'ns3::LteUeRrc::ImsiCidRntiCountTracedCallback&')
    typehandlers.add_type_alias(u'void ( * ) ( ns3::LteUeRrc::SlChangeOfSyncRefStatParameters const )', u'ns3::LteUeRrc::ChangeOfSyncRefTracedCallback')
    typehandlers.add_type_alias(u'void ( * ) ( ns3::LteUeRrc::SlChangeOfSyncRefStatParameters const )*', u'ns3::LteUeRrc::ChangeOfSyncRefTracedCallback*')
    typehandlers.add_type_alias(u'void ( * ) ( ns3::LteUeRrc::SlChangeOfSyncRefStatParameters const )&', u'ns3::LteUeRrc::ChangeOfSyncRefTracedCallback&')
    typehandlers.add_type_alias(u'void ( * ) ( uint64_t const, uint64_t const, uint16_t const, bool const, uint16_t const, uint16_t const )', u'ns3::LteUeRrc::SendSLSSTracedCallback')
    typehandlers.add_type_alias(u'void ( * ) ( uint64_t const, uint64_t const, uint16_t const, bool const, uint16_t const, uint16_t const )*', u'ns3::LteUeRrc::SendSLSSTracedCallback*')
    typehandlers.add_type_alias(u'void ( * ) ( uint64_t const, uint64_t const, uint16_t const, bool const, uint16_t const, uint16_t const )&', u'ns3::LteUeRrc::SendSLSSTracedCallback&')
    ## lte-rrc-protocol-ideal.h (module 'lte'): ns3::LteUeRrcProtocolIdeal [class]
    module.add_class('LteUeRrcProtocolIdeal', parent=root_module['ns3::Object'])
    ## lte-rrc-protocol-real.h (module 'lte'): ns3::LteUeRrcProtocolReal [class]
    module.add_class('LteUeRrcProtocolReal', parent=root_module['ns3::Object'])
    ## mac48-address.h (module 'network'): ns3::Mac48AddressChecker [class]
    module.add_class('Mac48AddressChecker', import_from_module='ns.network', parent=root_module['ns3::AttributeChecker'])
    ## mac48-address.h (module 'network'): ns3::Mac48AddressValue [class]
    module.add_class('Mac48AddressValue', import_from_module='ns.network', parent=root_module['ns3::AttributeValue'])
    ## mac64-address.h (module 'network'): ns3::Mac64AddressChecker [class]
    module.add_class('Mac64AddressChecker', import_from_module='ns.network', parent=root_module['ns3::AttributeChecker'])
    ## mac64-address.h (module 'network'): ns3::Mac64AddressValue [class]
    module.add_class('Mac64AddressValue', import_from_module='ns.network', parent=root_module['ns3::AttributeValue'])
    ## mac-stats-calculator.h (module 'lte'): ns3::MacStatsCalculator [class]
    module.add_class('MacStatsCalculator', parent=root_module['ns3::LteStatsCalculator'])
    ## lte-rrc-header.h (module 'lte'): ns3::MasterInformationBlockSlHeader [class]
    module.add_class('MasterInformationBlockSlHeader', parent=root_module['ns3::Asn1Header'])
    ## propagation-loss-model.h (module 'propagation'): ns3::MatrixPropagationLossModel [class]
    module.add_class('MatrixPropagationLossModel', import_from_module='ns.propagation', parent=root_module['ns3::PropagationLossModel'])
    ## lte-control-messages.h (module 'lte'): ns3::MibLteControlMessage [class]
    module.add_class('MibLteControlMessage', parent=root_module['ns3::LteControlMessage'])
    ## basic-data-calculators.h (module 'stats'): ns3::MinMaxAvgTotalCalculator<unsigned int> [class]
    module.add_class('MinMaxAvgTotalCalculator', import_from_module='ns.stats', template_parameters=['unsigned int'], parent=[root_module['ns3::DataCalculator'], root_module['ns3::StatisticalSummary']])
    ## basic-data-calculators.h (module 'stats'): ns3::MinMaxAvgTotalCalculator<unsigned long> [class]
    module.add_class('MinMaxAvgTotalCalculator', import_from_module='ns.stats', template_parameters=['unsigned long'], parent=[root_module['ns3::DataCalculator'], root_module['ns3::StatisticalSummary']])
    ## mobility-model.h (module 'mobility'): ns3::MobilityModel [class]
    module.add_class('MobilityModel', import_from_module='ns.mobility', parent=root_module['ns3::Object'])
    typehandlers.add_type_alias(u'void ( * ) ( ns3::Ptr< ns3::MobilityModel const > )', u'ns3::MobilityModel::TracedCallback')
    typehandlers.add_type_alias(u'void ( * ) ( ns3::Ptr< ns3::MobilityModel const > )*', u'ns3::MobilityModel::TracedCallback*')
    typehandlers.add_type_alias(u'void ( * ) ( ns3::Ptr< ns3::MobilityModel const > )&', u'ns3::MobilityModel::TracedCallback&')
    ## propagation-loss-model.h (module 'propagation'): ns3::NakagamiPropagationLossModel [class]
    module.add_class('NakagamiPropagationLossModel', import_from_module='ns.propagation', parent=root_module['ns3::PropagationLossModel'])
    ## net-device.h (module 'network'): ns3::NetDevice [class]
    module.add_class('NetDevice', import_from_module='ns.network', parent=root_module['ns3::Object'])
    ## net-device.h (module 'network'): ns3::NetDevice::PacketType [enumeration]
    module.add_enum('PacketType', ['PACKET_HOST', 'NS3_PACKET_HOST', 'PACKET_BROADCAST', 'NS3_PACKET_BROADCAST', 'PACKET_MULTICAST', 'NS3_PACKET_MULTICAST', 'PACKET_OTHERHOST', 'NS3_PACKET_OTHERHOST'], outer_class=root_module['ns3::NetDevice'], import_from_module='ns.network')
    typehandlers.add_type_alias(u'void ( * ) (  )', u'ns3::NetDevice::LinkChangeTracedCallback')
    typehandlers.add_type_alias(u'void ( * ) (  )*', u'ns3::NetDevice::LinkChangeTracedCallback*')
    typehandlers.add_type_alias(u'void ( * ) (  )&', u'ns3::NetDevice::LinkChangeTracedCallback&')
    typehandlers.add_type_alias(u'ns3::Callback< bool, ns3::Ptr< ns3::NetDevice >, ns3::Ptr< ns3::Packet const >, unsigned short, ns3::Address const &, ns3::empty, ns3::empty, ns3::empty, ns3::empty, ns3::empty >', u'ns3::NetDevice::ReceiveCallback')
    typehandlers.add_type_alias(u'ns3::Callback< bool, ns3::Ptr< ns3::NetDevice >, ns3::Ptr< ns3::Packet const >, unsigned short, ns3::Address const &, ns3::empty, ns3::empty, ns3::empty, ns3::empty, ns3::empty >*', u'ns3::NetDevice::ReceiveCallback*')
    typehandlers.add_type_alias(u'ns3::Callback< bool, ns3::Ptr< ns3::NetDevice >, ns3::Ptr< ns3::Packet const >, unsigned short, ns3::Address const &, ns3::empty, ns3::empty, ns3::empty, ns3::empty, ns3::empty >&', u'ns3::NetDevice::ReceiveCallback&')
    typehandlers.add_type_alias(u'ns3::Callback< bool, ns3::Ptr< ns3::NetDevice >, ns3::Ptr< ns3::Packet const >, unsigned short, ns3::Address const &, ns3::Address const &, ns3::NetDevice::PacketType, ns3::empty, ns3::empty, ns3::empty >', u'ns3::NetDevice::PromiscReceiveCallback')
    typehandlers.add_type_alias(u'ns3::Callback< bool, ns3::Ptr< ns3::NetDevice >, ns3::Ptr< ns3::Packet const >, unsigned short, ns3::Address const &, ns3::Address const &, ns3::NetDevice::PacketType, ns3::empty, ns3::empty, ns3::empty >*', u'ns3::NetDevice::PromiscReceiveCallback*')
    typehandlers.add_type_alias(u'ns3::Callback< bool, ns3::Ptr< ns3::NetDevice >, ns3::Ptr< ns3::Packet const >, unsigned short, ns3::Address const &, ns3::Address const &, ns3::NetDevice::PacketType, ns3::empty, ns3::empty, ns3::empty >&', u'ns3::NetDevice::PromiscReceiveCallback&')
    ## nix-vector.h (module 'network'): ns3::NixVector [class]
    module.add_class('NixVector', import_from_module='ns.network', parent=root_module['ns3::SimpleRefCount< ns3::NixVector, ns3::empty, ns3::DefaultDeleter<ns3::NixVector> >'])
    ## no-backhaul-epc-helper.h (module 'lte'): ns3::NoBackhaulEpcHelper [class]
    module.add_class('NoBackhaulEpcHelper', parent=root_module['ns3::EpcHelper'])
    ## no-op-component-carrier-manager.h (module 'lte'): ns3::NoOpComponentCarrierManager [class]
    module.add_class('NoOpComponentCarrierManager', parent=root_module['ns3::LteEnbComponentCarrierManager'])
    ## no-op-handover-algorithm.h (module 'lte'): ns3::NoOpHandoverAlgorithm [class]
    module.add_class('NoOpHandoverAlgorithm', parent=root_module['ns3::LteHandoverAlgorithm'])
    ## node.h (module 'network'): ns3::Node [class]
    module.add_class('Node', import_from_module='ns.network', parent=root_module['ns3::Object'])
    typehandlers.add_type_alias(u'ns3::Callback< void, ns3::Ptr< ns3::NetDevice >, ns3::Ptr< ns3::Packet const >, unsigned short, ns3::Address const &, ns3::Address const &, ns3::NetDevice::PacketType, ns3::empty, ns3::empty, ns3::empty >', u'ns3::Node::ProtocolHandler')
    typehandlers.add_type_alias(u'ns3::Callback< void, ns3::Ptr< ns3::NetDevice >, ns3::Ptr< ns3::Packet const >, unsigned short, ns3::Address const &, ns3::Address const &, ns3::NetDevice::PacketType, ns3::empty, ns3::empty, ns3::empty >*', u'ns3::Node::ProtocolHandler*')
    typehandlers.add_type_alias(u'ns3::Callback< void, ns3::Ptr< ns3::NetDevice >, ns3::Ptr< ns3::Packet const >, unsigned short, ns3::Address const &, ns3::Address const &, ns3::NetDevice::PacketType, ns3::empty, ns3::empty, ns3::empty >&', u'ns3::Node::ProtocolHandler&')
    typehandlers.add_type_alias(u'ns3::Callback< void, ns3::Ptr< ns3::NetDevice >, ns3::empty, ns3::empty, ns3::empty, ns3::empty, ns3::empty, ns3::empty, ns3::empty, ns3::empty >', u'ns3::Node::DeviceAdditionListener')
    typehandlers.add_type_alias(u'ns3::Callback< void, ns3::Ptr< ns3::NetDevice >, ns3::empty, ns3::empty, ns3::empty, ns3::empty, ns3::empty, ns3::empty, ns3::empty, ns3::empty >*', u'ns3::Node::DeviceAdditionListener*')
    typehandlers.add_type_alias(u'ns3::Callback< void, ns3::Ptr< ns3::NetDevice >, ns3::empty, ns3::empty, ns3::empty, ns3::empty, ns3::empty, ns3::empty, ns3::empty, ns3::empty >&', u'ns3::Node::DeviceAdditionListener&')
    ## random-variable-stream.h (module 'core'): ns3::NormalRandomVariable [class]
    module.add_class('NormalRandomVariable', import_from_module='ns.core', parent=root_module['ns3::RandomVariableStream'])
    ## object-factory.h (module 'core'): ns3::ObjectFactoryChecker [class]
    module.add_class('ObjectFactoryChecker', import_from_module='ns.core', parent=root_module['ns3::AttributeChecker'])
    ## object-factory.h (module 'core'): ns3::ObjectFactoryValue [class]
    module.add_class('ObjectFactoryValue', import_from_module='ns.core', parent=root_module['ns3::AttributeValue'])
    ## output-stream-wrapper.h (module 'network'): ns3::OutputStreamWrapper [class]
    module.add_class('OutputStreamWrapper', import_from_module='ns.network', parent=root_module['ns3::SimpleRefCount< ns3::OutputStreamWrapper, ns3::empty, ns3::DefaultDeleter<ns3::OutputStreamWrapper> >'])
    ## packet.h (module 'network'): ns3::Packet [class]
    module.add_class('Packet', import_from_module='ns.network', parent=root_module['ns3::SimpleRefCount< ns3::Packet, ns3::empty, ns3::DefaultDeleter<ns3::Packet> >'])
    typehandlers.add_type_alias(u'void ( * ) ( ns3::Ptr< ns3::Packet const > )', u'ns3::Packet::TracedCallback')
    typehandlers.add_type_alias(u'void ( * ) ( ns3::Ptr< ns3::Packet const > )*', u'ns3::Packet::TracedCallback*')
    typehandlers.add_type_alias(u'void ( * ) ( ns3::Ptr< ns3::Packet const > )&', u'ns3::Packet::TracedCallback&')
    typehandlers.add_type_alias(u'void ( * ) ( ns3::Ptr< ns3::Packet const >, ns3::Address const & )', u'ns3::Packet::AddressTracedCallback')
    typehandlers.add_type_alias(u'void ( * ) ( ns3::Ptr< ns3::Packet const >, ns3::Address const & )*', u'ns3::Packet::AddressTracedCallback*')
    typehandlers.add_type_alias(u'void ( * ) ( ns3::Ptr< ns3::Packet const >, ns3::Address const & )&', u'ns3::Packet::AddressTracedCallback&')
    typehandlers.add_type_alias(u'void ( * ) ( ns3::Ptr< ns3::Packet const > const, ns3::Address const &, ns3::Address const & )', u'ns3::Packet::TwoAddressTracedCallback')
    typehandlers.add_type_alias(u'void ( * ) ( ns3::Ptr< ns3::Packet const > const, ns3::Address const &, ns3::Address const & )*', u'ns3::Packet::TwoAddressTracedCallback*')
    typehandlers.add_type_alias(u'void ( * ) ( ns3::Ptr< ns3::Packet const > const, ns3::Address const &, ns3::Address const & )&', u'ns3::Packet::TwoAddressTracedCallback&')
    typehandlers.add_type_alias(u'void ( * ) ( ns3::Ptr< ns3::Packet const >, ns3::Mac48Address )', u'ns3::Packet::Mac48AddressTracedCallback')
    typehandlers.add_type_alias(u'void ( * ) ( ns3::Ptr< ns3::Packet const >, ns3::Mac48Address )*', u'ns3::Packet::Mac48AddressTracedCallback*')
    typehandlers.add_type_alias(u'void ( * ) ( ns3::Ptr< ns3::Packet const >, ns3::Mac48Address )&', u'ns3::Packet::Mac48AddressTracedCallback&')
    typehandlers.add_type_alias(u'void ( * ) ( uint32_t, uint32_t )', u'ns3::Packet::SizeTracedCallback')
    typehandlers.add_type_alias(u'void ( * ) ( uint32_t, uint32_t )*', u'ns3::Packet::SizeTracedCallback*')
    typehandlers.add_type_alias(u'void ( * ) ( uint32_t, uint32_t )&', u'ns3::Packet::SizeTracedCallback&')
    typehandlers.add_type_alias(u'void ( * ) ( ns3::Ptr< ns3::Packet const >, double )', u'ns3::Packet::SinrTracedCallback')
    typehandlers.add_type_alias(u'void ( * ) ( ns3::Ptr< ns3::Packet const >, double )*', u'ns3::Packet::SinrTracedCallback*')
    typehandlers.add_type_alias(u'void ( * ) ( ns3::Ptr< ns3::Packet const >, double )&', u'ns3::Packet::SinrTracedCallback&')
    ## random-variable-stream.h (module 'core'): ns3::ParetoRandomVariable [class]
    module.add_class('ParetoRandomVariable', import_from_module='ns.core', parent=root_module['ns3::RandomVariableStream'])
    ## pf-ff-mac-scheduler.h (module 'lte'): ns3::PfFfMacScheduler [class]
    module.add_class('PfFfMacScheduler', parent=root_module['ns3::FfMacScheduler'])
    ## phy-rx-stats-calculator.h (module 'lte'): ns3::PhyRxStatsCalculator [class]
    module.add_class('PhyRxStatsCalculator', parent=root_module['ns3::LteStatsCalculator'])
    ## phy-stats-calculator.h (module 'lte'): ns3::PhyStatsCalculator [class]
    module.add_class('PhyStatsCalculator', parent=root_module['ns3::LteStatsCalculator'])
    ## phy-tx-stats-calculator.h (module 'lte'): ns3::PhyTxStatsCalculator [class]
    module.add_class('PhyTxStatsCalculator', parent=root_module['ns3::LteStatsCalculator'])
    ## point-to-point-epc-helper.h (module 'lte'): ns3::PointToPointEpcHelper [class]
    module.add_class('PointToPointEpcHelper', parent=root_module['ns3::NoBackhaulEpcHelper'])
    ## pointer.h (module 'core'): ns3::PointerChecker [class]
    module.add_class('PointerChecker', import_from_module='ns.core', parent=root_module['ns3::AttributeChecker'])
    ## pointer.h (module 'core'): ns3::PointerValue [class]
    module.add_class('PointerValue', import_from_module='ns.core', parent=root_module['ns3::AttributeValue'])
    ## pss-ff-mac-scheduler.h (module 'lte'): ns3::PssFfMacScheduler [class]
    module.add_class('PssFfMacScheduler', parent=root_module['ns3::FfMacScheduler'])
    ## lte-control-messages.h (module 'lte'): ns3::RachPreambleLteControlMessage [class]
    module.add_class('RachPreambleLteControlMessage', parent=root_module['ns3::LteControlMessage'])
    ## radio-bearer-stats-calculator.h (module 'lte'): ns3::RadioBearerStatsCalculator [class]
    module.add_class('RadioBearerStatsCalculator', parent=root_module['ns3::LteStatsCalculator'])
    ## lte-control-messages.h (module 'lte'): ns3::RarLteControlMessage [class]
    module.add_class('RarLteControlMessage', parent=root_module['ns3::LteControlMessage'])
    ## lte-control-messages.h (module 'lte'): ns3::RarLteControlMessage::Rar [struct]
    module.add_class('Rar', outer_class=root_module['ns3::RarLteControlMessage'])
    ## rem-spectrum-phy.h (module 'lte'): ns3::RemSpectrumPhy [class]
    module.add_class('RemSpectrumPhy', parent=root_module['ns3::SpectrumPhy'])
    ## no-op-component-carrier-manager.h (module 'lte'): ns3::RrComponentCarrierManager [class]
    module.add_class('RrComponentCarrierManager', parent=root_module['ns3::NoOpComponentCarrierManager'])
    ## rr-ff-mac-scheduler.h (module 'lte'): ns3::RrFfMacScheduler [class]
    module.add_class('RrFfMacScheduler', parent=root_module['ns3::FfMacScheduler'])
    ## rr-sl-ff-mac-scheduler.h (module 'lte'): ns3::RrSlFfMacScheduler [class]
    module.add_class('RrSlFfMacScheduler', parent=root_module['ns3::FfMacScheduler'])
    ## lte-rrc-header.h (module 'lte'): ns3::RrcAsn1Header [class]
    module.add_class('RrcAsn1Header', parent=root_module['ns3::Asn1Header'])
    ## lte-rrc-header.h (module 'lte'): ns3::RrcDlCcchMessage [class]
    module.add_class('RrcDlCcchMessage', parent=root_module['ns3::RrcAsn1Header'])
    ## lte-rrc-header.h (module 'lte'): ns3::RrcDlDcchMessage [class]
    module.add_class('RrcDlDcchMessage', parent=root_module['ns3::RrcAsn1Header'])
    ## rrc-stats-calculator.h (module 'lte'): ns3::RrcStatsCalculator [class]
    module.add_class('RrcStatsCalculator', parent=root_module['ns3::LteStatsCalculator'])
    ## lte-rrc-header.h (module 'lte'): ns3::RrcUlCcchMessage [class]
    module.add_class('RrcUlCcchMessage', parent=root_module['ns3::RrcAsn1Header'])
    ## lte-rrc-header.h (module 'lte'): ns3::RrcUlDcchMessage [class]
    module.add_class('RrcUlDcchMessage', parent=root_module['ns3::RrcAsn1Header'])
    ## lte-control-messages.h (module 'lte'): ns3::Sib1LteControlMessage [class]
    module.add_class('Sib1LteControlMessage', parent=root_module['ns3::LteControlMessage'])
    ## lte-rrc-header.h (module 'lte'): ns3::SidelinkUeInformationHeader [class]
    module.add_class('SidelinkUeInformationHeader', parent=root_module['ns3::RrcUlDcchMessage'])
    ## simple-ue-component-carrier-manager.h (module 'lte'): ns3::SimpleUeComponentCarrierManager [class]
    module.add_class('SimpleUeComponentCarrierManager', parent=root_module['ns3::LteUeComponentCarrierManager'])
    ## lte-control-messages.h (module 'lte'): ns3::SlDciLteControlMessage [class]
    module.add_class('SlDciLteControlMessage', parent=root_module['ns3::LteControlMessage'])
    ## spectrum-channel.h (module 'spectrum'): ns3::SpectrumChannel [class]
    module.add_class('SpectrumChannel', import_from_module='ns.spectrum', parent=root_module['ns3::Channel'])
    typehandlers.add_type_alias(u'void ( * ) ( ns3::Ptr< ns3::SpectrumPhy const >, ns3::Ptr< ns3::SpectrumPhy const >, double )', u'ns3::SpectrumChannel::LossTracedCallback')
    typehandlers.add_type_alias(u'void ( * ) ( ns3::Ptr< ns3::SpectrumPhy const >, ns3::Ptr< ns3::SpectrumPhy const >, double )*', u'ns3::SpectrumChannel::LossTracedCallback*')
    typehandlers.add_type_alias(u'void ( * ) ( ns3::Ptr< ns3::SpectrumPhy const >, ns3::Ptr< ns3::SpectrumPhy const >, double )&', u'ns3::SpectrumChannel::LossTracedCallback&')
    typehandlers.add_type_alias(u'void ( * ) ( ns3::Ptr< ns3::MobilityModel const >, ns3::Ptr< ns3::MobilityModel const >, double, double, double, double )', u'ns3::SpectrumChannel::GainTracedCallback')
    typehandlers.add_type_alias(u'void ( * ) ( ns3::Ptr< ns3::MobilityModel const >, ns3::Ptr< ns3::MobilityModel const >, double, double, double, double )*', u'ns3::SpectrumChannel::GainTracedCallback*')
    typehandlers.add_type_alias(u'void ( * ) ( ns3::Ptr< ns3::MobilityModel const >, ns3::Ptr< ns3::MobilityModel const >, double, double, double, double )&', u'ns3::SpectrumChannel::GainTracedCallback&')
    typehandlers.add_type_alias(u'void ( * ) ( ns3::Ptr< ns3::SpectrumSignalParameters > )', u'ns3::SpectrumChannel::SignalParametersTracedCallback')
    typehandlers.add_type_alias(u'void ( * ) ( ns3::Ptr< ns3::SpectrumSignalParameters > )*', u'ns3::SpectrumChannel::SignalParametersTracedCallback*')
    typehandlers.add_type_alias(u'void ( * ) ( ns3::Ptr< ns3::SpectrumSignalParameters > )&', u'ns3::SpectrumChannel::SignalParametersTracedCallback&')
    ## lte-vendor-specific-parameters.h (module 'lte'): ns3::SrsCqiRntiVsp [class]
    module.add_class('SrsCqiRntiVsp', parent=root_module['ns3::VendorSpecificValue'])
    ## string.h (module 'core'): ns3::StringChecker [class]
    module.add_class('StringChecker', import_from_module='ns.core', parent=root_module['ns3::AttributeChecker'])
    ## string.h (module 'core'): ns3::StringValue [class]
    module.add_class('StringValue', import_from_module='ns.core', parent=root_module['ns3::AttributeValue'])
    ## tdbet-ff-mac-scheduler.h (module 'lte'): ns3::TdBetFfMacScheduler [class]
    module.add_class('TdBetFfMacScheduler', parent=root_module['ns3::FfMacScheduler'])
    ## tdmt-ff-mac-scheduler.h (module 'lte'): ns3::TdMtFfMacScheduler [class]
    module.add_class('TdMtFfMacScheduler', parent=root_module['ns3::FfMacScheduler'])
    ## tdtbfq-ff-mac-scheduler.h (module 'lte'): ns3::TdTbfqFfMacScheduler [class]
    module.add_class('TdTbfqFfMacScheduler', parent=root_module['ns3::FfMacScheduler'])
    ## nstime.h (module 'core'): ns3::TimeValue [class]
    module.add_class('TimeValue', import_from_module='ns.core', parent=root_module['ns3::AttributeValue'])
    ## tta-ff-mac-scheduler.h (module 'lte'): ns3::TtaFfMacScheduler [class]
    module.add_class('TtaFfMacScheduler', parent=root_module['ns3::FfMacScheduler'])
    ## type-id.h (module 'core'): ns3::TypeIdChecker [class]
    module.add_class('TypeIdChecker', import_from_module='ns.core', parent=root_module['ns3::AttributeChecker'])
    ## type-id.h (module 'core'): ns3::TypeIdValue [class]
    module.add_class('TypeIdValue', import_from_module='ns.core', parent=root_module['ns3::AttributeValue'])
    ## uinteger.h (module 'core'): ns3::UintegerValue [class]
    module.add_class('UintegerValue', import_from_module='ns.core', parent=root_module['ns3::AttributeValue'])
    ## lte-control-messages.h (module 'lte'): ns3::UlDciLteControlMessage [class]
    module.add_class('UlDciLteControlMessage', parent=root_module['ns3::LteControlMessage'])
    ## vector.h (module 'core'): ns3::Vector2DChecker [class]
    module.add_class('Vector2DChecker', import_from_module='ns.core', parent=root_module['ns3::AttributeChecker'])
    ## vector.h (module 'core'): ns3::Vector2DValue [class]
    module.add_class('Vector2DValue', import_from_module='ns.core', parent=root_module['ns3::AttributeValue'])
    ## vector.h (module 'core'): ns3::Vector3DChecker [class]
    module.add_class('Vector3DChecker', import_from_module='ns.core', parent=root_module['ns3::AttributeChecker'])
    ## vector.h (module 'core'): ns3::Vector3DValue [class]
    module.add_class('Vector3DValue', import_from_module='ns.core', parent=root_module['ns3::AttributeValue'])
    ## virtual-net-device.h (module 'virtual-net-device'): ns3::VirtualNetDevice [class]
    module.add_class('VirtualNetDevice', import_from_module='ns.virtual_net_device', parent=root_module['ns3::NetDevice'])
    typehandlers.add_type_alias(u'ns3::Callback< bool, ns3::Ptr< ns3::Packet >, ns3::Address const &, ns3::Address const &, unsigned short, ns3::empty, ns3::empty, ns3::empty, ns3::empty, ns3::empty >', u'ns3::VirtualNetDevice::SendCallback')
    typehandlers.add_type_alias(u'ns3::Callback< bool, ns3::Ptr< ns3::Packet >, ns3::Address const &, ns3::Address const &, unsigned short, ns3::empty, ns3::empty, ns3::empty, ns3::empty, ns3::empty >*', u'ns3::VirtualNetDevice::SendCallback*')
    typehandlers.add_type_alias(u'ns3::Callback< bool, ns3::Ptr< ns3::Packet >, ns3::Address const &, ns3::Address const &, unsigned short, ns3::empty, ns3::empty, ns3::empty, ns3::empty, ns3::empty >&', u'ns3::VirtualNetDevice::SendCallback&')
    ## a2-a4-rsrq-handover-algorithm.h (module 'lte'): ns3::A2A4RsrqHandoverAlgorithm [class]
    module.add_class('A2A4RsrqHandoverAlgorithm', parent=root_module['ns3::LteHandoverAlgorithm'])
    ## a3-rsrp-handover-algorithm.h (module 'lte'): ns3::A3RsrpHandoverAlgorithm [class]
    module.add_class('A3RsrpHandoverAlgorithm', parent=root_module['ns3::LteHandoverAlgorithm'])
    ## address.h (module 'network'): ns3::AddressChecker [class]
    module.add_class('AddressChecker', import_from_module='ns.network', parent=root_module['ns3::AttributeChecker'])
    ## address.h (module 'network'): ns3::AddressValue [class]
    module.add_class('AddressValue', import_from_module='ns.network', parent=root_module['ns3::AttributeValue'])
    ## lte-control-messages.h (module 'lte'): ns3::BsrLteControlMessage [class]
    module.add_class('BsrLteControlMessage', parent=root_module['ns3::LteControlMessage'])
    ## callback.h (module 'core'): ns3::CallbackImpl<bool, ns3::Ptr<ns3::NetDevice>, ns3::Ptr<const ns3::Packet>, unsigned short, const ns3::Address &, const ns3::Address &, ns3::NetDevice::PacketType, ns3::empty, ns3::empty, ns3::empty> [class]
    module.add_class('CallbackImpl', import_from_module='ns.core', template_parameters=['bool', 'ns3::Ptr<ns3::NetDevice>', 'ns3::Ptr<const ns3::Packet>', 'unsigned short', 'const ns3::Address &', 'const ns3::Address &', 'ns3::NetDevice::PacketType', 'ns3::empty', 'ns3::empty', 'ns3::empty'], parent=root_module['ns3::CallbackImplBase'])
    ## callback.h (module 'core'): ns3::CallbackImpl<bool, ns3::Ptr<ns3::NetDevice>, ns3::Ptr<const ns3::Packet>, unsigned short, const ns3::Address &, ns3::empty, ns3::empty, ns3::empty, ns3::empty, ns3::empty> [class]
    module.add_class('CallbackImpl', import_from_module='ns.core', template_parameters=['bool', 'ns3::Ptr<ns3::NetDevice>', 'ns3::Ptr<const ns3::Packet>', 'unsigned short', 'const ns3::Address &', 'ns3::empty', 'ns3::empty', 'ns3::empty', 'ns3::empty', 'ns3::empty'], parent=root_module['ns3::CallbackImplBase'])
    ## callback.h (module 'core'): ns3::CallbackImpl<bool, ns3::Ptr<ns3::Packet>, const ns3::Address &, const ns3::Address &, unsigned short, ns3::empty, ns3::empty, ns3::empty, ns3::empty, ns3::empty> [class]
    module.add_class('CallbackImpl', import_from_module='ns.core', template_parameters=['bool', 'ns3::Ptr<ns3::Packet>', 'const ns3::Address &', 'const ns3::Address &', 'unsigned short', 'ns3::empty', 'ns3::empty', 'ns3::empty', 'ns3::empty', 'ns3::empty'], parent=root_module['ns3::CallbackImplBase'])
    ## callback.h (module 'core'): ns3::CallbackImpl<bool, ns3::Ptr<ns3::Socket>, const ns3::Address &, ns3::empty, ns3::empty, ns3::empty, ns3::empty, ns3::empty, ns3::empty, ns3::empty> [class]
    module.add_class('CallbackImpl', import_from_module='ns.core', template_parameters=['bool', 'ns3::Ptr<ns3::Socket>', 'const ns3::Address &', 'ns3::empty', 'ns3::empty', 'ns3::empty', 'ns3::empty', 'ns3::empty', 'ns3::empty', 'ns3::empty'], parent=root_module['ns3::CallbackImplBase'])
    ## callback.h (module 'core'): ns3::CallbackImpl<ns3::ObjectBase *, ns3::empty, ns3::empty, ns3::empty, ns3::empty, ns3::empty, ns3::empty, ns3::empty, ns3::empty, ns3::empty> [class]
    module.add_class('CallbackImpl', import_from_module='ns.core', template_parameters=['ns3::ObjectBase *', 'ns3::empty', 'ns3::empty', 'ns3::empty', 'ns3::empty', 'ns3::empty', 'ns3::empty', 'ns3::empty', 'ns3::empty', 'ns3::empty'], parent=root_module['ns3::CallbackImplBase'])
    ## callback.h (module 'core'): ns3::CallbackImpl<void, const ns3::SpectrumValue &, ns3::empty, ns3::empty, ns3::empty, ns3::empty, ns3::empty, ns3::empty, ns3::empty, ns3::empty> [class]
    module.add_class('CallbackImpl', import_from_module='ns.core', template_parameters=['void', 'const ns3::SpectrumValue &', 'ns3::empty', 'ns3::empty', 'ns3::empty', 'ns3::empty', 'ns3::empty', 'ns3::empty', 'ns3::empty', 'ns3::empty'], parent=root_module['ns3::CallbackImplBase'])
    ## callback.h (module 'core'): ns3::CallbackImpl<void, ns3::DlSchedulingCallbackInfo, ns3::empty, ns3::empty, ns3::empty, ns3::empty, ns3::empty, ns3::empty, ns3::empty, ns3::empty> [class]
    module.add_class('CallbackImpl', template_parameters=['void', 'ns3::DlSchedulingCallbackInfo', 'ns3::empty', 'ns3::empty', 'ns3::empty', 'ns3::empty', 'ns3::empty', 'ns3::empty', 'ns3::empty', 'ns3::empty'], parent=root_module['ns3::CallbackImplBase'])
    ## callback.h (module 'core'): ns3::CallbackImpl<void, ns3::EpcUeNas::State, ns3::EpcUeNas::State, ns3::empty, ns3::empty, ns3::empty, ns3::empty, ns3::empty, ns3::empty, ns3::empty> [class]
    module.add_class('CallbackImpl', template_parameters=['void', 'ns3::EpcUeNas::State', 'ns3::EpcUeNas::State', 'ns3::empty', 'ns3::empty', 'ns3::empty', 'ns3::empty', 'ns3::empty', 'ns3::empty', 'ns3::empty'], parent=root_module['ns3::CallbackImplBase'])
    ## callback.h (module 'core'): ns3::CallbackImpl<void, ns3::LteUeRrc::SlChangeOfSyncRefStatParameters, ns3::empty, ns3::empty, ns3::empty, ns3::empty, ns3::empty, ns3::empty, ns3::empty, ns3::empty> [class]
    module.add_class('CallbackImpl', template_parameters=['void', 'ns3::LteUeRrc::SlChangeOfSyncRefStatParameters', 'ns3::empty', 'ns3::empty', 'ns3::empty', 'ns3::empty', 'ns3::empty', 'ns3::empty', 'ns3::empty', 'ns3::empty'], parent=root_module['ns3::CallbackImplBase'])
    ## callback.h (module 'core'): ns3::CallbackImpl<void, ns3::PhyReceptionStatParameters, ns3::empty, ns3::empty, ns3::empty, ns3::empty, ns3::empty, ns3::empty, ns3::empty, ns3::empty> [class]
    module.add_class('CallbackImpl', template_parameters=['void', 'ns3::PhyReceptionStatParameters', 'ns3::empty', 'ns3::empty', 'ns3::empty', 'ns3::empty', 'ns3::empty', 'ns3::empty', 'ns3::empty', 'ns3::empty'], parent=root_module['ns3::CallbackImplBase'])
    ## callback.h (module 'core'): ns3::CallbackImpl<void, ns3::PhyTransmissionStatParameters, ns3::empty, ns3::empty, ns3::empty, ns3::empty, ns3::empty, ns3::empty, ns3::empty, ns3::empty> [class]
    module.add_class('CallbackImpl', template_parameters=['void', 'ns3::PhyTransmissionStatParameters', 'ns3::empty', 'ns3::empty', 'ns3::empty', 'ns3::empty', 'ns3::empty', 'ns3::empty', 'ns3::empty', 'ns3::empty'], parent=root_module['ns3::CallbackImplBase'])
    ## callback.h (module 'core'): ns3::CallbackImpl<void, ns3::Ptr<const ns3::MobilityModel>, ns3::Ptr<const ns3::MobilityModel>, double, double, double, double, ns3::empty, ns3::empty, ns3::empty> [class]
    module.add_class('CallbackImpl', import_from_module='ns.core', template_parameters=['void', 'ns3::Ptr<const ns3::MobilityModel>', 'ns3::Ptr<const ns3::MobilityModel>', 'double', 'double', 'double', 'double', 'ns3::empty', 'ns3::empty', 'ns3::empty'], parent=root_module['ns3::CallbackImplBase'])
    ## callback.h (module 'core'): ns3::CallbackImpl<void, ns3::Ptr<const ns3::MobilityModel>, ns3::empty, ns3::empty, ns3::empty, ns3::empty, ns3::empty, ns3::empty, ns3::empty, ns3::empty> [class]
    module.add_class('CallbackImpl', import_from_module='ns.core', template_parameters=['void', 'ns3::Ptr<const ns3::MobilityModel>', 'ns3::empty', 'ns3::empty', 'ns3::empty', 'ns3::empty', 'ns3::empty', 'ns3::empty', 'ns3::empty', 'ns3::empty'], parent=root_module['ns3::CallbackImplBase'])
    ## callback.h (module 'core'): ns3::CallbackImpl<void, ns3::Ptr<const ns3::Packet>, ns3::empty, ns3::empty, ns3::empty, ns3::empty, ns3::empty, ns3::empty, ns3::empty, ns3::empty> [class]
    module.add_class('CallbackImpl', import_from_module='ns.core', template_parameters=['void', 'ns3::Ptr<const ns3::Packet>', 'ns3::empty', 'ns3::empty', 'ns3::empty', 'ns3::empty', 'ns3::empty', 'ns3::empty', 'ns3::empty', 'ns3::empty'], parent=root_module['ns3::CallbackImplBase'])
    ## callback.h (module 'core'): ns3::CallbackImpl<void, ns3::Ptr<const ns3::PacketBurst>, ns3::empty, ns3::empty, ns3::empty, ns3::empty, ns3::empty, ns3::empty, ns3::empty, ns3::empty> [class]
    module.add_class('CallbackImpl', import_from_module='ns.core', template_parameters=['void', 'ns3::Ptr<const ns3::PacketBurst>', 'ns3::empty', 'ns3::empty', 'ns3::empty', 'ns3::empty', 'ns3::empty', 'ns3::empty', 'ns3::empty', 'ns3::empty'], parent=root_module['ns3::CallbackImplBase'])
    ## callback.h (module 'core'): ns3::CallbackImpl<void, ns3::Ptr<const ns3::SpectrumPhy>, ns3::Ptr<const ns3::SpectrumPhy>, double, ns3::empty, ns3::empty, ns3::empty, ns3::empty, ns3::empty, ns3::empty> [class]
    module.add_class('CallbackImpl', import_from_module='ns.core', template_parameters=['void', 'ns3::Ptr<const ns3::SpectrumPhy>', 'ns3::Ptr<const ns3::SpectrumPhy>', 'double', 'ns3::empty', 'ns3::empty', 'ns3::empty', 'ns3::empty', 'ns3::empty', 'ns3::empty'], parent=root_module['ns3::CallbackImplBase'])
    ## callback.h (module 'core'): ns3::CallbackImpl<void, ns3::Ptr<ns3::LteSpectrumPhy>, ns3::empty, ns3::empty, ns3::empty, ns3::empty, ns3::empty, ns3::empty, ns3::empty, ns3::empty> [class]
    module.add_class('CallbackImpl', import_from_module='ns.core', template_parameters=['void', 'ns3::Ptr<ns3::LteSpectrumPhy>', 'ns3::empty', 'ns3::empty', 'ns3::empty', 'ns3::empty', 'ns3::empty', 'ns3::empty', 'ns3::empty', 'ns3::empty'], parent=root_module['ns3::CallbackImplBase'])
    ## callback.h (module 'core'): ns3::CallbackImpl<void, ns3::Ptr<ns3::LteUeRrc>, std::list<ns3::LteRrcSap::SCellToAddMod, std::allocator<ns3::LteRrcSap::SCellToAddMod> >, ns3::empty, ns3::empty, ns3::empty, ns3::empty, ns3::empty, ns3::empty, ns3::empty> [class]
    module.add_class('CallbackImpl', import_from_module='ns.core', template_parameters=['void', 'ns3::Ptr<ns3::LteUeRrc>', 'std::list<ns3::LteRrcSap::SCellToAddMod, std::allocator<ns3::LteRrcSap::SCellToAddMod> >', 'ns3::empty', 'ns3::empty', 'ns3::empty', 'ns3::empty', 'ns3::empty', 'ns3::empty', 'ns3::empty'], parent=root_module['ns3::CallbackImplBase'])
    ## callback.h (module 'core'): ns3::CallbackImpl<void, ns3::Ptr<ns3::NetDevice>, ns3::Ptr<const ns3::Packet>, unsigned short, const ns3::Address &, const ns3::Address &, ns3::NetDevice::PacketType, ns3::empty, ns3::empty, ns3::empty> [class]
    module.add_class('CallbackImpl', import_from_module='ns.core', template_parameters=['void', 'ns3::Ptr<ns3::NetDevice>', 'ns3::Ptr<const ns3::Packet>', 'unsigned short', 'const ns3::Address &', 'const ns3::Address &', 'ns3::NetDevice::PacketType', 'ns3::empty', 'ns3::empty', 'ns3::empty'], parent=root_module['ns3::CallbackImplBase'])
    ## callback.h (module 'core'): ns3::CallbackImpl<void, ns3::Ptr<ns3::NetDevice>, ns3::empty, ns3::empty, ns3::empty, ns3::empty, ns3::empty, ns3::empty, ns3::empty, ns3::empty> [class]
    module.add_class('CallbackImpl', import_from_module='ns.core', template_parameters=['void', 'ns3::Ptr<ns3::NetDevice>', 'ns3::empty', 'ns3::empty', 'ns3::empty', 'ns3::empty', 'ns3::empty', 'ns3::empty', 'ns3::empty', 'ns3::empty'], parent=root_module['ns3::CallbackImplBase'])
    ## callback.h (module 'core'): ns3::CallbackImpl<void, ns3::Ptr<ns3::Packet>, ns3::empty, ns3::empty, ns3::empty, ns3::empty, ns3::empty, ns3::empty, ns3::empty, ns3::empty> [class]
    module.add_class('CallbackImpl', import_from_module='ns.core', template_parameters=['void', 'ns3::Ptr<ns3::Packet>', 'ns3::empty', 'ns3::empty', 'ns3::empty', 'ns3::empty', 'ns3::empty', 'ns3::empty', 'ns3::empty', 'ns3::empty'], parent=root_module['ns3::CallbackImplBase'])
    ## callback.h (module 'core'): ns3::CallbackImpl<void, ns3::Ptr<ns3::Socket>, const ns3::Address &, ns3::empty, ns3::empty, ns3::empty, ns3::empty, ns3::empty, ns3::empty, ns3::empty> [class]
    module.add_class('CallbackImpl', import_from_module='ns.core', template_parameters=['void', 'ns3::Ptr<ns3::Socket>', 'const ns3::Address &', 'ns3::empty', 'ns3::empty', 'ns3::empty', 'ns3::empty', 'ns3::empty', 'ns3::empty', 'ns3::empty'], parent=root_module['ns3::CallbackImplBase'])
    ## callback.h (module 'core'): ns3::CallbackImpl<void, ns3::Ptr<ns3::Socket>, ns3::empty, ns3::empty, ns3::empty, ns3::empty, ns3::empty, ns3::empty, ns3::empty, ns3::empty> [class]
    module.add_class('CallbackImpl', import_from_module='ns.core', template_parameters=['void', 'ns3::Ptr<ns3::Socket>', 'ns3::empty', 'ns3::empty', 'ns3::empty', 'ns3::empty', 'ns3::empty', 'ns3::empty', 'ns3::empty', 'ns3::empty'], parent=root_module['ns3::CallbackImplBase'])
    ## callback.h (module 'core'): ns3::CallbackImpl<void, ns3::Ptr<ns3::Socket>, unsigned int, ns3::empty, ns3::empty, ns3::empty, ns3::empty, ns3::empty, ns3::empty, ns3::empty> [class]
    module.add_class('CallbackImpl', import_from_module='ns.core', template_parameters=['void', 'ns3::Ptr<ns3::Socket>', 'unsigned int', 'ns3::empty', 'ns3::empty', 'ns3::empty', 'ns3::empty', 'ns3::empty', 'ns3::empty', 'ns3::empty'], parent=root_module['ns3::CallbackImplBase'])
    ## callback.h (module 'core'): ns3::CallbackImpl<void, ns3::Ptr<ns3::SpectrumSignalParameters>, ns3::empty, ns3::empty, ns3::empty, ns3::empty, ns3::empty, ns3::empty, ns3::empty, ns3::empty> [class]
    module.add_class('CallbackImpl', import_from_module='ns.core', template_parameters=['void', 'ns3::Ptr<ns3::SpectrumSignalParameters>', 'ns3::empty', 'ns3::empty', 'ns3::empty', 'ns3::empty', 'ns3::empty', 'ns3::empty', 'ns3::empty', 'ns3::empty'], parent=root_module['ns3::CallbackImplBase'])
    ## callback.h (module 'core'): ns3::CallbackImpl<void, ns3::SlPhyReceptionStatParameters, ns3::empty, ns3::empty, ns3::empty, ns3::empty, ns3::empty, ns3::empty, ns3::empty, ns3::empty> [class]
    module.add_class('CallbackImpl', template_parameters=['void', 'ns3::SlPhyReceptionStatParameters', 'ns3::empty', 'ns3::empty', 'ns3::empty', 'ns3::empty', 'ns3::empty', 'ns3::empty', 'ns3::empty', 'ns3::empty'], parent=root_module['ns3::CallbackImplBase'])
    ## callback.h (module 'core'): ns3::CallbackImpl<void, ns3::SlUeMacStatParameters, ns3::LteSlDiscHeader, ns3::empty, ns3::empty, ns3::empty, ns3::empty, ns3::empty, ns3::empty, ns3::empty> [class]
    module.add_class('CallbackImpl', template_parameters=['void', 'ns3::SlUeMacStatParameters', 'ns3::LteSlDiscHeader', 'ns3::empty', 'ns3::empty', 'ns3::empty', 'ns3::empty', 'ns3::empty', 'ns3::empty', 'ns3::empty'], parent=root_module['ns3::CallbackImplBase'])
    ## callback.h (module 'core'): ns3::CallbackImpl<void, ns3::SlUeMacStatParameters, ns3::empty, ns3::empty, ns3::empty, ns3::empty, ns3::empty, ns3::empty, ns3::empty, ns3::empty> [class]
    module.add_class('CallbackImpl', template_parameters=['void', 'ns3::SlUeMacStatParameters', 'ns3::empty', 'ns3::empty', 'ns3::empty', 'ns3::empty', 'ns3::empty', 'ns3::empty', 'ns3::empty', 'ns3::empty'], parent=root_module['ns3::CallbackImplBase'])
    ## callback.h (module 'core'): ns3::CallbackImpl<void, ns3::empty, ns3::empty, ns3::empty, ns3::empty, ns3::empty, ns3::empty, ns3::empty, ns3::empty, ns3::empty> [class]
    module.add_class('CallbackImpl', import_from_module='ns.core', template_parameters=['void', 'ns3::empty', 'ns3::empty', 'ns3::empty', 'ns3::empty', 'ns3::empty', 'ns3::empty', 'ns3::empty', 'ns3::empty', 'ns3::empty'], parent=root_module['ns3::CallbackImplBase'])
    ## callback.h (module 'core'): ns3::CallbackImpl<void, std::vector<ns3::SpectrumValue, std::allocator<ns3::SpectrumValue> >, ns3::empty, ns3::empty, ns3::empty, ns3::empty, ns3::empty, ns3::empty, ns3::empty, ns3::empty> [class]
    module.add_class('CallbackImpl', import_from_module='ns.core', template_parameters=['void', 'std::vector<ns3::SpectrumValue, std::allocator<ns3::SpectrumValue> >', 'ns3::empty', 'ns3::empty', 'ns3::empty', 'ns3::empty', 'ns3::empty', 'ns3::empty', 'ns3::empty', 'ns3::empty'], parent=root_module['ns3::CallbackImplBase'])
    ## callback.h (module 'core'): ns3::CallbackImpl<void, unsigned int, unsigned int, unsigned int, ns3::empty, ns3::empty, ns3::empty, ns3::empty, ns3::empty, ns3::empty> [class]
    module.add_class('CallbackImpl', import_from_module='ns.core', template_parameters=['void', 'unsigned int', 'unsigned int', 'unsigned int', 'ns3::empty', 'ns3::empty', 'ns3::empty', 'ns3::empty', 'ns3::empty', 'ns3::empty'], parent=root_module['ns3::CallbackImplBase'])
    ## callback.h (module 'core'): ns3::CallbackImpl<void, unsigned int, unsigned int, unsigned short, unsigned char, unsigned short, unsigned char, ns3::empty, ns3::empty, ns3::empty> [class]
    module.add_class('CallbackImpl', import_from_module='ns.core', template_parameters=['void', 'unsigned int', 'unsigned int', 'unsigned short', 'unsigned char', 'unsigned short', 'unsigned char', 'ns3::empty', 'ns3::empty', 'ns3::empty'], parent=root_module['ns3::CallbackImplBase'])
    ## callback.h (module 'core'): ns3::CallbackImpl<void, unsigned long, bool, unsigned char, unsigned char, ns3::empty, ns3::empty, ns3::empty, ns3::empty, ns3::empty> [class]
    module.add_class('CallbackImpl', import_from_module='ns.core', template_parameters=['void', 'unsigned long', 'bool', 'unsigned char', 'unsigned char', 'ns3::empty', 'ns3::empty', 'ns3::empty', 'ns3::empty', 'ns3::empty'], parent=root_module['ns3::CallbackImplBase'])
    ## callback.h (module 'core'): ns3::CallbackImpl<void, unsigned long, unsigned long, unsigned short, bool, unsigned short, unsigned short, ns3::empty, ns3::empty, ns3::empty> [class]
    module.add_class('CallbackImpl', import_from_module='ns.core', template_parameters=['void', 'unsigned long', 'unsigned long', 'unsigned short', 'bool', 'unsigned short', 'unsigned short', 'ns3::empty', 'ns3::empty', 'ns3::empty'], parent=root_module['ns3::CallbackImplBase'])
    ## callback.h (module 'core'): ns3::CallbackImpl<void, unsigned long, unsigned short, ns3::empty, ns3::empty, ns3::empty, ns3::empty, ns3::empty, ns3::empty, ns3::empty> [class]
    module.add_class('CallbackImpl', import_from_module='ns.core', template_parameters=['void', 'unsigned long', 'unsigned short', 'ns3::empty', 'ns3::empty', 'ns3::empty', 'ns3::empty', 'ns3::empty', 'ns3::empty', 'ns3::empty'], parent=root_module['ns3::CallbackImplBase'])
    ## callback.h (module 'core'): ns3::CallbackImpl<void, unsigned long, unsigned short, unsigned short, ns3::LteRrcSap::MeasurementReport, ns3::empty, ns3::empty, ns3::empty, ns3::empty, ns3::empty> [class]
    module.add_class('CallbackImpl', template_parameters=['void', 'unsigned long', 'unsigned short', 'unsigned short', 'ns3::LteRrcSap::MeasurementReport', 'ns3::empty', 'ns3::empty', 'ns3::empty', 'ns3::empty', 'ns3::empty'], parent=root_module['ns3::CallbackImplBase'])
    ## callback.h (module 'core'): ns3::CallbackImpl<void, unsigned long, unsigned short, unsigned short, ns3::LteSlDiscHeader, ns3::empty, ns3::empty, ns3::empty, ns3::empty, ns3::empty> [class]
    module.add_class('CallbackImpl', template_parameters=['void', 'unsigned long', 'unsigned short', 'unsigned short', 'ns3::LteSlDiscHeader', 'ns3::empty', 'ns3::empty', 'ns3::empty', 'ns3::empty', 'ns3::empty'], parent=root_module['ns3::CallbackImplBase'])
    ## callback.h (module 'core'): ns3::CallbackImpl<void, unsigned long, unsigned short, unsigned short, ns3::LteUeRrc::State, ns3::LteUeRrc::State, ns3::empty, ns3::empty, ns3::empty, ns3::empty> [class]
    module.add_class('CallbackImpl', template_parameters=['void', 'unsigned long', 'unsigned short', 'unsigned short', 'ns3::LteUeRrc::State', 'ns3::LteUeRrc::State', 'ns3::empty', 'ns3::empty', 'ns3::empty', 'ns3::empty'], parent=root_module['ns3::CallbackImplBase'])
    ## callback.h (module 'core'): ns3::CallbackImpl<void, unsigned long, unsigned short, unsigned short, ns3::UeManager::State, ns3::UeManager::State, ns3::empty, ns3::empty, ns3::empty, ns3::empty> [class]
    module.add_class('CallbackImpl', template_parameters=['void', 'unsigned long', 'unsigned short', 'unsigned short', 'ns3::UeManager::State', 'ns3::UeManager::State', 'ns3::empty', 'ns3::empty', 'ns3::empty', 'ns3::empty'], parent=root_module['ns3::CallbackImplBase'])
    ## callback.h (module 'core'): ns3::CallbackImpl<void, unsigned long, unsigned short, unsigned short, ns3::empty, ns3::empty, ns3::empty, ns3::empty, ns3::empty, ns3::empty> [class]
    module.add_class('CallbackImpl', import_from_module='ns.core', template_parameters=['void', 'unsigned long', 'unsigned short', 'unsigned short', 'ns3::empty', 'ns3::empty', 'ns3::empty', 'ns3::empty', 'ns3::empty', 'ns3::empty'], parent=root_module['ns3::CallbackImplBase'])
    ## callback.h (module 'core'): ns3::CallbackImpl<void, unsigned long, unsigned short, unsigned short, std::basic_string<char>, ns3::empty, ns3::empty, ns3::empty, ns3::empty, ns3::empty> [class]
    module.add_class('CallbackImpl', import_from_module='ns.core', template_parameters=['void', 'unsigned long', 'unsigned short', 'unsigned short', 'std::basic_string<char>', 'ns3::empty', 'ns3::empty', 'ns3::empty', 'ns3::empty', 'ns3::empty'], parent=root_module['ns3::CallbackImplBase'])
    ## callback.h (module 'core'): ns3::CallbackImpl<void, unsigned long, unsigned short, unsigned short, std::basic_string<char>, unsigned char, ns3::empty, ns3::empty, ns3::empty, ns3::empty> [class]
    module.add_class('CallbackImpl', import_from_module='ns.core', template_parameters=['void', 'unsigned long', 'unsigned short', 'unsigned short', 'std::basic_string<char>', 'unsigned char', 'ns3::empty', 'ns3::empty', 'ns3::empty', 'ns3::empty'], parent=root_module['ns3::CallbackImplBase'])
    ## callback.h (module 'core'): ns3::CallbackImpl<void, unsigned long, unsigned short, unsigned short, unsigned char, ns3::empty, ns3::empty, ns3::empty, ns3::empty, ns3::empty> [class]
    module.add_class('CallbackImpl', import_from_module='ns.core', template_parameters=['void', 'unsigned long', 'unsigned short', 'unsigned short', 'unsigned char', 'ns3::empty', 'ns3::empty', 'ns3::empty', 'ns3::empty', 'ns3::empty'], parent=root_module['ns3::CallbackImplBase'])
    ## callback.h (module 'core'): ns3::CallbackImpl<void, unsigned long, unsigned short, unsigned short, unsigned short, ns3::empty, ns3::empty, ns3::empty, ns3::empty, ns3::empty> [class]
    module.add_class('CallbackImpl', import_from_module='ns.core', template_parameters=['void', 'unsigned long', 'unsigned short', 'unsigned short', 'unsigned short', 'ns3::empty', 'ns3::empty', 'ns3::empty', 'ns3::empty', 'ns3::empty'], parent=root_module['ns3::CallbackImplBase'])
    ## callback.h (module 'core'): ns3::CallbackImpl<void, unsigned short, ns3::Ptr<ns3::SpectrumValue>, ns3::empty, ns3::empty, ns3::empty, ns3::empty, ns3::empty, ns3::empty, ns3::empty> [class]
    module.add_class('CallbackImpl', import_from_module='ns.core', template_parameters=['void', 'unsigned short', 'ns3::Ptr<ns3::SpectrumValue>', 'ns3::empty', 'ns3::empty', 'ns3::empty', 'ns3::empty', 'ns3::empty', 'ns3::empty', 'ns3::empty'], parent=root_module['ns3::CallbackImplBase'])
    ## callback.h (module 'core'): ns3::CallbackImpl<void, unsigned short, unsigned char, unsigned int, ns3::empty, ns3::empty, ns3::empty, ns3::empty, ns3::empty, ns3::empty> [class]
    module.add_class('CallbackImpl', import_from_module='ns.core', template_parameters=['void', 'unsigned short', 'unsigned char', 'unsigned int', 'ns3::empty', 'ns3::empty', 'ns3::empty', 'ns3::empty', 'ns3::empty', 'ns3::empty'], parent=root_module['ns3::CallbackImplBase'])
    ## callback.h (module 'core'): ns3::CallbackImpl<void, unsigned short, unsigned char, unsigned int, unsigned long, ns3::empty, ns3::empty, ns3::empty, ns3::empty, ns3::empty> [class]
    module.add_class('CallbackImpl', import_from_module='ns.core', template_parameters=['void', 'unsigned short', 'unsigned char', 'unsigned int', 'unsigned long', 'ns3::empty', 'ns3::empty', 'ns3::empty', 'ns3::empty', 'ns3::empty'], parent=root_module['ns3::CallbackImplBase'])
    ## callback.h (module 'core'): ns3::CallbackImpl<void, unsigned short, unsigned short, double, double, bool, unsigned char, ns3::empty, ns3::empty, ns3::empty> [class]
    module.add_class('CallbackImpl', import_from_module='ns.core', template_parameters=['void', 'unsigned short', 'unsigned short', 'double', 'double', 'bool', 'unsigned char', 'ns3::empty', 'ns3::empty', 'ns3::empty'], parent=root_module['ns3::CallbackImplBase'])
    ## callback.h (module 'core'): ns3::CallbackImpl<void, unsigned short, unsigned short, double, double, unsigned char, ns3::empty, ns3::empty, ns3::empty, ns3::empty> [class]
    module.add_class('CallbackImpl', import_from_module='ns.core', template_parameters=['void', 'unsigned short', 'unsigned short', 'double', 'double', 'unsigned char', 'ns3::empty', 'ns3::empty', 'ns3::empty', 'ns3::empty'], parent=root_module['ns3::CallbackImplBase'])
    ## callback.h (module 'core'): ns3::CallbackImpl<void, unsigned short, unsigned short, double, ns3::empty, ns3::empty, ns3::empty, ns3::empty, ns3::empty, ns3::empty> [class]
    module.add_class('CallbackImpl', import_from_module='ns.core', template_parameters=['void', 'unsigned short', 'unsigned short', 'double', 'ns3::empty', 'ns3::empty', 'ns3::empty', 'ns3::empty', 'ns3::empty', 'ns3::empty'], parent=root_module['ns3::CallbackImplBase'])
    ## callback.h (module 'core'): ns3::CallbackImpl<void, unsigned short, unsigned short, double, unsigned char, ns3::empty, ns3::empty, ns3::empty, ns3::empty, ns3::empty> [class]
    module.add_class('CallbackImpl', import_from_module='ns.core', template_parameters=['void', 'unsigned short', 'unsigned short', 'double', 'unsigned char', 'ns3::empty', 'ns3::empty', 'ns3::empty', 'ns3::empty', 'ns3::empty'], parent=root_module['ns3::CallbackImplBase'])
    ## callback.h (module 'core'): ns3::CallbackImpl<void, unsigned short, unsigned short, ns3::LteUePhy::State, ns3::LteUePhy::State, ns3::empty, ns3::empty, ns3::empty, ns3::empty, ns3::empty> [class]
    module.add_class('CallbackImpl', template_parameters=['void', 'unsigned short', 'unsigned short', 'ns3::LteUePhy::State', 'ns3::LteUePhy::State', 'ns3::empty', 'ns3::empty', 'ns3::empty', 'ns3::empty', 'ns3::empty'], parent=root_module['ns3::CallbackImplBase'])
    ## callback.h (module 'core'): ns3::CallbackImpl<void, unsigned short, unsigned short, ns3::empty, ns3::empty, ns3::empty, ns3::empty, ns3::empty, ns3::empty, ns3::empty> [class]
    module.add_class('CallbackImpl', import_from_module='ns.core', template_parameters=['void', 'unsigned short', 'unsigned short', 'ns3::empty', 'ns3::empty', 'ns3::empty', 'ns3::empty', 'ns3::empty', 'ns3::empty', 'ns3::empty'], parent=root_module['ns3::CallbackImplBase'])
    ## cqa-ff-mac-scheduler.h (module 'lte'): ns3::CqaFfMacScheduler [class]
    module.add_class('CqaFfMacScheduler', parent=root_module['ns3::FfMacScheduler'])
    ## lte-control-messages.h (module 'lte'): ns3::DlCqiLteControlMessage [class]
    module.add_class('DlCqiLteControlMessage', parent=root_module['ns3::LteControlMessage'])
    ## lte-control-messages.h (module 'lte'): ns3::DlDciLteControlMessage [class]
    module.add_class('DlDciLteControlMessage', parent=root_module['ns3::LteControlMessage'])
    ## lte-control-messages.h (module 'lte'): ns3::DlHarqFeedbackLteControlMessage [class]
    module.add_class('DlHarqFeedbackLteControlMessage', parent=root_module['ns3::LteControlMessage'])
    ## emu-epc-helper.h (module 'lte'): ns3::EmuEpcHelper [class]
    module.add_class('EmuEpcHelper', parent=root_module['ns3::NoBackhaulEpcHelper'])
    ## fdbet-ff-mac-scheduler.h (module 'lte'): ns3::FdBetFfMacScheduler [class]
    module.add_class('FdBetFfMacScheduler', parent=root_module['ns3::FfMacScheduler'])
    ## fdmt-ff-mac-scheduler.h (module 'lte'): ns3::FdMtFfMacScheduler [class]
    module.add_class('FdMtFfMacScheduler', parent=root_module['ns3::FfMacScheduler'])
    ## fdtbfq-ff-mac-scheduler.h (module 'lte'): ns3::FdTbfqFfMacScheduler [class]
    module.add_class('FdTbfqFfMacScheduler', parent=root_module['ns3::FfMacScheduler'])
    ## epc-gtpc-header.h (module 'lte'): ns3::GtpcCreateSessionRequestMessage [class]
    module.add_class('GtpcCreateSessionRequestMessage', parent=[root_module['ns3::GtpcHeader'], root_module['ns3::GtpcIes']])
    ## epc-gtpc-header.h (module 'lte'): ns3::GtpcCreateSessionRequestMessage::BearerContextToBeCreated [struct]
    module.add_class('BearerContextToBeCreated', outer_class=root_module['ns3::GtpcCreateSessionRequestMessage'])
    ## epc-gtpc-header.h (module 'lte'): ns3::GtpcCreateSessionResponseMessage [class]
    module.add_class('GtpcCreateSessionResponseMessage', parent=[root_module['ns3::GtpcHeader'], root_module['ns3::GtpcIes']])
    ## epc-gtpc-header.h (module 'lte'): ns3::GtpcCreateSessionResponseMessage::BearerContextCreated [struct]
    module.add_class('BearerContextCreated', outer_class=root_module['ns3::GtpcCreateSessionResponseMessage'])
    ## epc-gtpc-header.h (module 'lte'): ns3::GtpcDeleteBearerCommandMessage [class]
    module.add_class('GtpcDeleteBearerCommandMessage', parent=[root_module['ns3::GtpcHeader'], root_module['ns3::GtpcIes']])
    ## epc-gtpc-header.h (module 'lte'): ns3::GtpcDeleteBearerCommandMessage::BearerContext [struct]
    module.add_class('BearerContext', outer_class=root_module['ns3::GtpcDeleteBearerCommandMessage'])
    ## epc-gtpc-header.h (module 'lte'): ns3::GtpcDeleteBearerRequestMessage [class]
    module.add_class('GtpcDeleteBearerRequestMessage', parent=[root_module['ns3::GtpcHeader'], root_module['ns3::GtpcIes']])
    ## epc-gtpc-header.h (module 'lte'): ns3::GtpcDeleteBearerResponseMessage [class]
    module.add_class('GtpcDeleteBearerResponseMessage', parent=[root_module['ns3::GtpcHeader'], root_module['ns3::GtpcIes']])
    ## lte-rrc-header.h (module 'lte'): ns3::HandoverPreparationInfoHeader [class]
    module.add_class('HandoverPreparationInfoHeader', parent=root_module['ns3::RrcAsn1Header'])
    ## lte-radio-bearer-info.h (module 'lte'): ns3::LteDataRadioBearerInfo [class]
    module.add_class('LteDataRadioBearerInfo', parent=root_module['ns3::LteRadioBearerInfo'])
    ## lte-enb-phy.h (module 'lte'): ns3::LteEnbPhy [class]
    module.add_class('LteEnbPhy', parent=root_module['ns3::LtePhy'])
    typehandlers.add_type_alias(u'void ( * ) ( uint16_t, uint16_t, double, uint8_t )', u'ns3::LteEnbPhy::ReportUeSinrTracedCallback')
    typehandlers.add_type_alias(u'void ( * ) ( uint16_t, uint16_t, double, uint8_t )*', u'ns3::LteEnbPhy::ReportUeSinrTracedCallback*')
    typehandlers.add_type_alias(u'void ( * ) ( uint16_t, uint16_t, double, uint8_t )&', u'ns3::LteEnbPhy::ReportUeSinrTracedCallback&')
    typehandlers.add_type_alias(u'void ( * ) ( uint16_t, ns3::Ptr< ns3::SpectrumValue > )', u'ns3::LteEnbPhy::ReportInterferenceTracedCallback')
    typehandlers.add_type_alias(u'void ( * ) ( uint16_t, ns3::Ptr< ns3::SpectrumValue > )*', u'ns3::LteEnbPhy::ReportInterferenceTracedCallback*')
    typehandlers.add_type_alias(u'void ( * ) ( uint16_t, ns3::Ptr< ns3::SpectrumValue > )&', u'ns3::LteEnbPhy::ReportInterferenceTracedCallback&')
    ## lte-net-device.h (module 'lte'): ns3::LteNetDevice [class]
    module.add_class('LteNetDevice', parent=root_module['ns3::NetDevice'])
    ## lte-spectrum-signal-parameters.h (module 'lte'): ns3::LteSpectrumSignalParametersSlCtrlFrame [struct]
    module.add_class('LteSpectrumSignalParametersSlCtrlFrame', parent=root_module['ns3::LteSpectrumSignalParametersSlFrame'])
    ## lte-spectrum-signal-parameters.h (module 'lte'): ns3::LteSpectrumSignalParametersSlDataFrame [struct]
    module.add_class('LteSpectrumSignalParametersSlDataFrame', parent=root_module['ns3::LteSpectrumSignalParametersSlFrame'])
    ## lte-spectrum-signal-parameters.h (module 'lte'): ns3::LteSpectrumSignalParametersSlDiscFrame [struct]
    module.add_class('LteSpectrumSignalParametersSlDiscFrame', parent=root_module['ns3::LteSpectrumSignalParametersSlFrame'])
    ## lte-ue-net-device.h (module 'lte'): ns3::LteUeNetDevice [class]
    module.add_class('LteUeNetDevice', parent=root_module['ns3::LteNetDevice'])
    ## lte-rrc-header.h (module 'lte'): ns3::MeasurementReportHeader [class]
    module.add_class('MeasurementReportHeader', parent=root_module['ns3::RrcUlDcchMessage'])
    ## lte-rrc-header.h (module 'lte'): ns3::RrcConnectionReconfigurationCompleteHeader [class]
    module.add_class('RrcConnectionReconfigurationCompleteHeader', parent=root_module['ns3::RrcUlDcchMessage'])
    ## lte-rrc-header.h (module 'lte'): ns3::RrcConnectionReconfigurationHeader [class]
    module.add_class('RrcConnectionReconfigurationHeader', parent=root_module['ns3::RrcDlDcchMessage'])
    ## lte-rrc-header.h (module 'lte'): ns3::RrcConnectionReestablishmentCompleteHeader [class]
    module.add_class('RrcConnectionReestablishmentCompleteHeader', parent=root_module['ns3::RrcUlDcchMessage'])
    ## lte-rrc-header.h (module 'lte'): ns3::RrcConnectionReestablishmentHeader [class]
    module.add_class('RrcConnectionReestablishmentHeader', parent=root_module['ns3::RrcDlCcchMessage'])
    ## lte-rrc-header.h (module 'lte'): ns3::RrcConnectionReestablishmentRejectHeader [class]
    module.add_class('RrcConnectionReestablishmentRejectHeader', parent=root_module['ns3::RrcDlCcchMessage'])
    ## lte-rrc-header.h (module 'lte'): ns3::RrcConnectionReestablishmentRequestHeader [class]
    module.add_class('RrcConnectionReestablishmentRequestHeader', parent=root_module['ns3::RrcUlCcchMessage'])
    ## lte-rrc-header.h (module 'lte'): ns3::RrcConnectionRejectHeader [class]
    module.add_class('RrcConnectionRejectHeader', parent=root_module['ns3::RrcDlCcchMessage'])
    ## lte-rrc-header.h (module 'lte'): ns3::RrcConnectionReleaseHeader [class]
    module.add_class('RrcConnectionReleaseHeader', parent=root_module['ns3::RrcDlDcchMessage'])
    ## lte-rrc-header.h (module 'lte'): ns3::RrcConnectionRequestHeader [class]
    module.add_class('RrcConnectionRequestHeader', parent=root_module['ns3::RrcUlCcchMessage'])
    ## lte-rrc-header.h (module 'lte'): ns3::RrcConnectionSetupCompleteHeader [class]
    module.add_class('RrcConnectionSetupCompleteHeader', parent=root_module['ns3::RrcUlDcchMessage'])
    ## lte-rrc-header.h (module 'lte'): ns3::RrcConnectionSetupHeader [class]
    module.add_class('RrcConnectionSetupHeader', parent=root_module['ns3::RrcDlCcchMessage'])
    ## lte-enb-net-device.h (module 'lte'): ns3::LteEnbNetDevice [class]
    module.add_class('LteEnbNetDevice', parent=root_module['ns3::LteNetDevice'])
    module.add_container('std::vector< ns3::CeBitmap_e >', 'ns3::CeBitmap_e', container_type=u'vector')
    module.add_container('std::vector< std::vector< ns3::RlcPduListElement_s > >', 'std::vector< ns3::RlcPduListElement_s >', container_type=u'vector')
    module.add_container('std::vector< unsigned char >', 'unsigned char', container_type=u'vector')
    module.add_container('std::vector< unsigned short >', 'short unsigned int', container_type=u'vector')
    module.add_container('std::vector< ns3::DlInfoListElement_s::HarqStatus_e >', 'ns3::DlInfoListElement_s::HarqStatus_e', container_type=u'vector')
    module.add_container('std::list< ns3::EpcEnbS1SapProvider::BearerToBeSwitched >', 'ns3::EpcEnbS1SapProvider::BearerToBeSwitched', container_type=u'list')
    module.add_container('std::list< ns3::EpcS11SapMme::BearerContextCreated >', 'ns3::EpcS11SapMme::BearerContextCreated', container_type=u'list')
    module.add_container('std::list< ns3::EpcS11SapMme::BearerContextRemoved >', 'ns3::EpcS11SapMme::BearerContextRemoved', container_type=u'list')
    module.add_container('std::list< ns3::EpcS11SapSgw::BearerContextToBeCreated >', 'ns3::EpcS11SapSgw::BearerContextToBeCreated', container_type=u'list')
    module.add_container('std::list< ns3::EpcS11SapSgw::BearerContextToBeRemoved >', 'ns3::EpcS11SapSgw::BearerContextToBeRemoved', container_type=u'list')
    module.add_container('std::list< ns3::EpcS11SapSgw::BearerContextRemovedSgwPgw >', 'ns3::EpcS11SapSgw::BearerContextRemovedSgwPgw', container_type=u'list')
    module.add_container('std::list< ns3::EpcS1apSapEnb::ErabToBeSetupItem >', 'ns3::EpcS1apSapEnb::ErabToBeSetupItem', container_type=u'list')
    module.add_container('std::list< ns3::EpcS1apSapEnb::ErabSwitchedInUplinkItem >', 'ns3::EpcS1apSapEnb::ErabSwitchedInUplinkItem', container_type=u'list')
    module.add_container('std::list< ns3::EpcS1apSapMme::ErabToBeReleasedIndication >', 'ns3::EpcS1apSapMme::ErabToBeReleasedIndication', container_type=u'list')
    module.add_container('std::list< ns3::EpcS1apSapMme::ErabSetupItem >', 'ns3::EpcS1apSapMme::ErabSetupItem', container_type=u'list')
    module.add_container('std::list< ns3::EpcS1apSapMme::ErabSwitchedInDownlinkItem >', 'ns3::EpcS1apSapMme::ErabSwitchedInDownlinkItem', container_type=u'list')
    module.add_container('std::vector< bool >', 'bool', container_type=u'vector')
    module.add_container('std::vector< ns3::EpcX2Sap::UlInterferenceOverloadIndicationItem >', 'ns3::EpcX2Sap::UlInterferenceOverloadIndicationItem', container_type=u'vector')
    module.add_container('std::vector< ns3::EpcX2Sap::UlHighInterferenceInformationItem >', 'ns3::EpcX2Sap::UlHighInterferenceInformationItem', container_type=u'vector')
    module.add_container('std::vector< ns3::EpcX2Sap::ErabToBeSetupItem >', 'ns3::EpcX2Sap::ErabToBeSetupItem', container_type=u'vector')
    module.add_container('std::vector< ns3::EpcX2Sap::ErabAdmittedItem >', 'ns3::EpcX2Sap::ErabAdmittedItem', container_type=u'vector')
    module.add_container('std::vector< ns3::EpcX2Sap::ErabNotAdmittedItem >', 'ns3::EpcX2Sap::ErabNotAdmittedItem', container_type=u'vector')
    module.add_container('std::vector< ns3::EpcX2Sap::ErabsSubjectToStatusTransferItem >', 'ns3::EpcX2Sap::ErabsSubjectToStatusTransferItem', container_type=u'vector')
    module.add_container('std::vector< ns3::EpcX2Sap::CellInformationItem >', 'ns3::EpcX2Sap::CellInformationItem', container_type=u'vector')
    module.add_container('std::vector< ns3::EpcX2Sap::CellMeasurementResultItem >', 'ns3::EpcX2Sap::CellMeasurementResultItem', container_type=u'vector')
    module.add_container('std::vector< ns3::VendorSpecificListElement_s >', 'ns3::VendorSpecificListElement_s', container_type=u'vector')
    module.add_container('std::vector< unsigned int >', 'unsigned int', container_type=u'vector')
    module.add_container('std::vector< ns3::LogicalChannelConfigListElement_s >', 'ns3::LogicalChannelConfigListElement_s', container_type=u'vector')
    module.add_container('std::vector< ns3::PagingInfoListElement_s >', 'ns3::PagingInfoListElement_s', container_type=u'vector')
    module.add_container('std::vector< ns3::DlInfoListElement_s >', 'ns3::DlInfoListElement_s', container_type=u'vector')
    module.add_container('std::vector< ns3::RachListElement_s >', 'ns3::RachListElement_s', container_type=u'vector')
    module.add_container('std::vector< ns3::CqiListElement_s >', 'ns3::CqiListElement_s', container_type=u'vector')
    module.add_container('std::vector< ns3::UlInfoListElement_s >', 'ns3::UlInfoListElement_s', container_type=u'vector')
    module.add_container('std::vector< ns3::SrListElement_s >', 'ns3::SrListElement_s', container_type=u'vector')
    module.add_container('std::vector< ns3::MacCeListElement_s >', 'ns3::MacCeListElement_s', container_type=u'vector')
    module.add_container('std::vector< ns3::BuildDataListElement_s >', 'ns3::BuildDataListElement_s', container_type=u'vector')
    module.add_container('std::vector< ns3::BuildRarListElement_s >', 'ns3::BuildRarListElement_s', container_type=u'vector')
    module.add_container('std::vector< ns3::BuildBroadcastListElement_s >', 'ns3::BuildBroadcastListElement_s', container_type=u'vector')
    module.add_container('std::vector< ns3::UlDciListElement_s >', 'ns3::UlDciListElement_s', container_type=u'vector')
    module.add_container('std::vector< ns3::PhichListElement_s >', 'ns3::PhichListElement_s', container_type=u'vector')
    module.add_container('std::vector< ns3::SlDciListElement_s >', 'ns3::SlDciListElement_s', container_type=u'vector')
    module.add_container('std::list< ns3::EpcTft::PacketFilter >', 'ns3::EpcTft::PacketFilter', container_type=u'list')
    module.add_container('std::map< std::string, ns3::LogComponent * >', ('std::string', 'ns3::LogComponent *'), container_type=u'map')
    module.add_container('std::vector< ns3::LteCcmRrcSapProvider::LcsConfig >', 'ns3::LteCcmRrcSapProvider::LcsConfig', container_type=u'vector')
    module.add_container('std::vector< ns3::LteRrcSap::LogicalChannelConfig >', 'ns3::LteRrcSap::LogicalChannelConfig', container_type=u'vector')
    module.add_container('std::map< unsigned short, std::vector< double > >', ('short unsigned int', 'std::vector< double >'), container_type=u'map')
    module.add_container('std::vector< int >', 'int', container_type=u'vector')
    module.add_container('ns3::HarqProcessInfoList_t', 'ns3::HarqProcessInfoElement_t', container_type=u'vector')
    module.add_container('std::list< ns3::LteRrcSap::SrbToAddMod >', 'ns3::LteRrcSap::SrbToAddMod', container_type=u'list')
    module.add_container('std::list< ns3::LteRrcSap::DrbToAddMod >', 'ns3::LteRrcSap::DrbToAddMod', container_type=u'list')
    module.add_container('std::list< unsigned char >', 'unsigned char', container_type=u'list')
    module.add_container('std::list< ns3::LteRrcSap::CellsToAddMod >', 'ns3::LteRrcSap::CellsToAddMod', container_type=u'list')
    module.add_container('std::list< ns3::LteRrcSap::BlackCellsToAddMod >', 'ns3::LteRrcSap::BlackCellsToAddMod', container_type=u'list')
    module.add_container('std::list< ns3::LteRrcSap::MeasObjectToAddMod >', 'ns3::LteRrcSap::MeasObjectToAddMod', container_type=u'list')
    module.add_container('std::list< ns3::LteRrcSap::ReportConfigToAddMod >', 'ns3::LteRrcSap::ReportConfigToAddMod', container_type=u'list')
    module.add_container('std::list< ns3::LteRrcSap::MeasIdToAddMod >', 'ns3::LteRrcSap::MeasIdToAddMod', container_type=u'list')
    module.add_container('std::list< unsigned int >', 'unsigned int', container_type=u'list')
    module.add_container('std::list< ns3::LteRrcSap::MeasResultScell >', 'ns3::LteRrcSap::MeasResultScell', container_type=u'list')
    module.add_container('std::list< ns3::LteRrcSap::MeasResultBestNeighCell >', 'ns3::LteRrcSap::MeasResultBestNeighCell', container_type=u'list')
    module.add_container('std::list< ns3::LteRrcSap::MeasResultEutra >', 'ns3::LteRrcSap::MeasResultEutra', container_type=u'list')
    module.add_container('std::list< ns3::LteRrcSap::SCellToAddMod >', 'ns3::LteRrcSap::SCellToAddMod', container_type=u'list')
    module.add_container('std::map< int, double >', ('int', 'double'), container_type=u'map')
    module.add_container('std::vector< ns3::LteUeCcmRrcSapProvider::LcsConfig >', 'ns3::LteUeCcmRrcSapProvider::LcsConfig', container_type=u'vector')
    module.add_container('std::list< ns3::Ptr< ns3::SidelinkRxCommResourcePool > >', 'ns3::Ptr< ns3::SidelinkRxCommResourcePool >', container_type=u'list')
    module.add_container('std::list< ns3::Ptr< ns3::SidelinkRxDiscResourcePool > >', 'ns3::Ptr< ns3::SidelinkRxDiscResourcePool >', container_type=u'list')
    module.add_container('std::vector< ns3::LteUeCphySapUser::UeMeasurementsElement >', 'ns3::LteUeCphySapUser::UeMeasurementsElement', container_type=u'vector')
    module.add_container('std::vector< ns3::LteUeCphySapUser::UeSlssMeasurementsElement >', 'ns3::LteUeCphySapUser::UeSlssMeasurementsElement', container_type=u'vector')
    module.add_container('std::vector< ns3::HigherLayerSelected_s >', 'ns3::HigherLayerSelected_s', container_type=u'vector')
    module.add_container('std::vector< ns3::SiMessageListElement_s >', 'ns3::SiMessageListElement_s', container_type=u'vector')
    module.add_container('std::vector< ns3::Vector3D >', 'ns3::Vector3D', container_type=u'vector')
    module.add_container('std::list< ns3::Ptr< ns3::Packet > >', 'ns3::Ptr< ns3::Packet >', container_type=u'list')
    module.add_container('std::list< ns3::SidelinkCommResourcePool::SidelinkTransmissionInfo >', 'ns3::SidelinkCommResourcePool::SidelinkTransmissionInfo', container_type=u'list')
    module.add_container('std::vector< std::vector< unsigned char > >', 'std::vector< unsigned char >', container_type=u'vector')
    module.add_container('std::list< ns3::SidelinkDiscResourcePool::SidelinkTransmissionInfo >', 'ns3::SidelinkDiscResourcePool::SidelinkTransmissionInfo', container_type=u'list')
    module.add_container('std::unordered_set< unsigned int >', 'unsigned int', container_type=u'hash_set')
    module.add_container('std::vector< ns3::Ipv6Address >', 'ns3::Ipv6Address', container_type=u'vector')
    module.add_container('std::vector< double >', 'double', container_type=u'vector')
    module.add_container('ns3::Bands', 'ns3::BandInfo', container_type=u'vector')
    module.add_container('std::map< unsigned char, ns3::ComponentCarrier >', ('unsigned char', 'ns3::ComponentCarrier'), container_type=u'map')
    module.add_container('std::list< ns3::GtpcModifyBearerRequestMessage::BearerContextToBeModified >', 'ns3::GtpcModifyBearerRequestMessage::BearerContextToBeModified', container_type=u'list')
    module.add_container('std::map< unsigned int, unsigned int >', ('unsigned int', 'unsigned int'), container_type=u'map')
    module.add_container('std::vector< ns3::Ptr< ns3::Building > >', 'ns3::Ptr< ns3::Building >', container_type=u'vector')
    module.add_container('std::map< std::vector< double >, ns3::WrapAroundReplicas >', ('std::vector< double >', 'ns3::WrapAroundReplicas'), container_type=u'map')
    module.add_container('std::map< unsigned long, ns3::WrapAroundInfo_t >', ('long unsigned int', 'ns3::WrapAroundInfo_t'), container_type=u'map')
    module.add_container('std::map< unsigned char, ns3::Ptr< ns3::ComponentCarrierBaseStation > >', ('unsigned char', 'ns3::Ptr< ns3::ComponentCarrierBaseStation >'), container_type=u'map')
    module.add_container('std::list< ns3::Ptr< ns3::LteControlMessage > >', 'ns3::Ptr< ns3::LteControlMessage >', container_type=u'list')
    module.add_container('std::vector< ns3::NetDeviceContainer >', 'ns3::NetDeviceContainer', container_type=u'vector')
