// ===========================================================================
//	MacHeaders.c				©1995-1998 Metrowerks Inc. All rights reserved.
// ===========================================================================
//
//	Includes used to generate the 'MacHeaders' precompiled header for
//  Metrowerks C/C++.
//
//	RA/8/19/98	Updated to Universal Headers 3.1
/*
 *	Required for c-style toolbox glue functions: c2pstr and p2cstr
 *	(the inverse operation (pointers_in_A0) is performed at the end...)
 */
#if __MC68K__ && !__CFM68K__
	#pragma d0_pointers on
#endif
/*
 *	To allow the use of ToolBox calls which have now become obsolete on
 *  PowerPC, but which are still needed for System 6 applications, we need to
 *  #define OBSOLETE.  If your application will never use these calls then you
 *  can comment out this #define.  NB: This is for 68K only ...
 *
 
#if !defined(powerc) && !defined(OBSOLETE)
	#define OBSOLETE	1
#endif
 */
#ifndef OLDROUTINENAMES
	#define	OLDROUTINENAMES 		0
#endif
#ifndef	OLDROUTINELOCATIONS
	#define OLDROUTINELOCATIONS 	0
#endif
/*
 *	Metrowerks-specific definitions
 *
 *	These definitions are commonly used but not in Apple's headers. We define
 *	them in our precompiled header so we can use the Apple headers without
 *  modification.
 */
#ifndef PtoCstr
	#define PtoCstr		p2cstr
#endif
#ifndef CtoPstr
	#define CtoPstr		c2pstr
#endif
#ifndef PtoCString
	#define PtoCString	p2cstr
#endif
#ifndef CtoPString
	#define CtoPString	c2pstr
#endif
#ifndef topLeft
	#define topLeft(r)	(((Point *) &(r))[0])
#endif
#ifndef botRight
	#define botRight(r)	(((Point *) &(r))[1])
#endif
#ifndef TRUE
	#define TRUE		true
#endif
#ifndef FALSE
	#define FALSE		false
#endif
/* RA/8/19/98 No longer necessary -- will be removed for Pro5
#ifndef powerc
	#include <MixedMode.h>
	long GetCurrentA5(void)
	ONEWORDINLINE(0x200D);
#endif
 */
/*
 *	Apple Universal Headers 3.1
 *
 *	Uncomment any additional #includes you want to add to your MacHeaders.
 */
//	#include <ADSP.h>
//	#include <ADSPSecure.h>
	#include <AEDataModel.h>
	#include <AEObjects.h>
	#include <AEPackObject.h>
	#include <AERegistry.h>
	#include <AEUserTermTypes.h>
//	#include <AIFF.h>
	#include <Aliases.h>
	#include <Appearance.h>
	#include <AppleEvents.h>
	#include <AppleGuide.h>
	#include <AppleScript.h>
	#include <AppleTalk.h>
	#include <ASDebugging.h>
	#include <ASRegistry.h>
//	#include <ATA.h>
//	#include <AVComponents.h>
	#include <Balloons.h>
//	#include <CardServices.h>
//	#include <CMAcceleration.h>
	#include <CMApplication.h>
//	#include <CMCalibrator.h>
//	#include <CMComponent.h>
//	#include <CMConversions.h>
	#include <CMICCProfile.h>
//	#include <CMMComponent.h>
//	#include <CMPRComponent.h>
//	#include <CMScriptingPlugin.h>
	#include <CodeFragments.h>
//	#include <Collections.h>
	#include <ColorPicker.h>
//	#include <ColorPickerComponents.h>
//	#include <CommResources.h>
	#include <Components.h>
	#include <ConditionalMacros.h>
//	#include <Connections.h>
//	#include <ConnectionTools.h>
	#include <Controls.h>
//	#include <ControlStrip.h>
//	#include <CRMSerialDevices.h>
//	#include <CTBUtilities.h>
//	#include <CursorDevices.h>
//	#include <DatabaseAccess.h>
	#include <DateTimeUtils.h>
//	#include <Desk.h>
//	#include <DeskBus.h>
	#include <Devices.h>
	#include <Dialogs.h>
//	#include <Dictionary.h>
//	#include <DigitalSignature.h>
	#include <DiskInit.h>
//	#include <Disks.h>
	#include <Displays.h>
	#include <Drag.h>
//	#include <DrawSprocket.h>
//	#include <DriverSynchronization.h>
	#include <DriverFamilyMatching.h>
//	#include <DriverGestalt.h>
//	#include <DriverServices.h>
//	#include <DriverSupport.h>
//	#include <Editions.h>
//	#include <Endian.h>
//	#include <ENET.h>
	#include <EPPC.h>
	#include <Errors.h>
	#include <Events.h>
//	#include <fenv.h>
	#include <Files.h>
//	#include <FileTransfers.h>
//	#include <FileTransferTools.h>
	#include <FileTypesAndCreators.h>
	#include <Finder.h>
	#include <FinderRegistry.h>
	#include <FixMath.h>
	#include <Folders.h>
	#include <Fonts.h>
//	#include <fp.h>
//	#include <FragLoad.h>
//	#include <FSM.h>
	#include <Gestalt.h>
//	#include <GXEnvironment.h>
//	#include <GXErrors.h>
//	#include <GXFonts.h>
//	#include <GXGraphics.h>
//	#include <GXLayout.h>
//	#include <GXMath.h>
//	#include <GXMessages.h>
//	#include <GXPrinterDrivers.h>
//	#include <GXPrinting.h>
//	#include <GXTypes.h>
//	#include <GoggleSprocket.h>
	#include <HFSVolumes.h>
//	#include <HyperXCmd.h>
	#include <Icons.h>
//	#include <ImageCodec.h>
	#include <ImageCompression.h>
//	#include <InputSprocket.h>
//	#include <Interrupts.h>
	#include <IntlResources.h>
//	#include <JManager.h>
//	#include <Kernel.h>
//	#include <Language.h>
	#include <Lists.h>
//	#include <LocationManager.h>
//	#include <LocationManager.k.h>
	#include <LowMem.h>
//	#include <MachineExceptions.h>
	#include <MacMemory.h>
//	#include <MacTCP.h>
	#include <MacTypes.h>
	#include <MacWindows.h>
//	#include <Math64.h>
//	#include <MediaHandlers.h>
	#include <Memory.h>						// Start using MacMemory.h
	#include <Menus.h>
//	#include <MIDI.h>
	#include <MixedMode.h>
	#include <Movies.h>
//	#include <MoviesFormat.h>
//	#include <MP.h>
//	#include <Multiprocessing.h>
	#include <NameRegistry.h>
//	#include <NetSprocket.h>
	#include <Notification.h>
	#include <NumberFormatting.h>
//	#include <OCE.h>
//	#include <OCEAuthDir.h>
//	#include <OCEErrors.h>
//	#include <OCEMail.h>
//	#include <OCEMessaging.h>
//	#include <OCEStandardDirectory.h>
//	#include <OCEStandardMail.h>
//	#include <OCETemplates.h>
	#include <OSA.h>
	#include <OSAComp.h>
	#include <OSAGeneric.h>
	#include <OSUtils.h>
//	#include <Packages.h>
	#include <Palettes.h>
	#include <Patches.h>
//	#include <PCCardAdapterPlugin.h>
//	#include <PCCardEnablerPlugin.h>
//	#include <PCCardTuples.h>
//	#include <PCI.h>
//	#include <PEFBinaryFormat.h>
//	#include <PCCard.h>
//	#include <Picker.h>
//	#include <PictUtil.h>
//	#include <PictUtils.h>
	#include <PLStringFuncs.h>
//	#include <Power.h>
	#include <PPCToolbox.h>
	#include <Printing.h>
	#include <Processes.h>
//	#include <QD3DAcceleration.h>
//	#include <QD3DCamera.h>
//	#include <QD3DController.h>
//	#include <QD3DCustomElements.h>
//	#include <QD3DDrawContext.h>
//	#include <QD3DErrors.h>
//	#include <QD3DExtension.h>
//	#include <QD3DGeometry.h>
//	#include <QD3DGroup.h>
//	#include <QD3DIO.h>
//	#include <QD3DLight.h>
//	#include <QD3DMath.h>
//	#include <QD3DPick.h>
//	#include <QD3DRenderer.h>
//	#include <QD3DSet.h>
//	#include <QD3DShader.h>
//	#include <QD3DStorage.h>
//	#include <QD3DString.h>
//	#include <QD3DStyle.h>
//	#include <QD3DTransform.h>
//	#include <QD3DView.h>
//	#include <QD3DViewer.h>
//	#include <QD3DWinViewer.h>
//	#include <QD3D.h>
	#include <QDOffscreen.h>
//	#include <QTML.h>
//	#include <QTMLCustomElements.h>
	#include <Quickdraw.h>
	#include <QuickdrawText.h>
//	#include <QuickTimeComponents.h>
//	#include <QuickTimeVR.h>
//	#include <QuickTimeVRFormat.h>
//	#include <RAVE.h>
//	#include <QuickTimeMusic.h>
	#include <Resources.h>
//	#include <Retrace.h>
//	#include <ROMDefs.h>
//	#include <ScalerTypes.h>
//	#include <ScalerStreamTypes.h>
	#include <Scrap.h>
	#include <Script.h>
//	#include <SCSI.h>
	#include <SegLoad.h>
//	#include <Serial.h>
//	#include <SFNTTypes.h>
//	#include <SFNTLayoutTypes.h>
//	#include <ShutDown.h>
//	#include <SIOW.h>
//	#include <Slots.h>
//	#include <SocketServices.h>
	#include <Sound.h>
//	#include <SoundComponents.h>
//	#include <SoundInput.h>
//	#include <SoundSprocket.h>
//	#include <Speech.h>
//	#include <SpeechRecognition.h>
//	#include <SpeechSynthesis.h>
	#include <StandardFile.h>
	#include <StringCompare.h>
//	#include <Start.h>
	#include <Strings.h>				// Start using TextUtils.h
//	#include <Telephones.h>
//	#include <Terminals.h>
//	#include <TerminalTools.h>
	#include <TextCommon.h>
	#include <TextEdit.h>
//	#include <TextEncodingConverter.h>
//	#include <TextServices.h>
	#include <TextUtils.h>
	#include <Threads.h>
	#include <Timer.h>
	#include <ToolUtils.h>
//	#include <Translation.h>
//	#include <TranslationExtensions.h>
	#include <Traps.h>
//	#include <TSMTE.h>
	#include <Types.h>					// Start using MacTypes.h
//	#include <Unicode.h>
//	#include <UnicodeConverter.h>
	#include <Video.h>
//	#include <VideoServices.h>
	#include <Windows.h>				// Start using MacWindows.h
//	#include <WorldScript.h>
//	#include <ZoomedVideo.h>
/*
 *	Required for c-style toolbox glue functions: c2pstr and p2cstr
 *	(matches the inverse operation at the start of the file...)
 */
#include <stdlib.h>
#include <unix.h>
#if __MC68K__ && !__CFM68K__
 #pragma d0_pointers reset
#endif
