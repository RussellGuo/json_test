using MHTool.Connector;
using ISPCore.Chip;
using ISPCore.Connect;
using ISPCore.Packet;
using ISPCore.Util;
using System;
using System.Windows;
using System.Collections.Generic;
using System.Threading;
using Org.BouncyCastle.Crypto;
using Org.BouncyCastle.Crypto.Digests;
using net.meano.Util;

namespace MHTool.UI
{
    public partial class MainWindow : Window
    {
        public bool ChipConnected(bool isConnected)
        {
            if (isConnected)
            {
                if (ConnectedChip == null)
                {
                    Logger.Error("The tool doesn't support the current connected chip!");
                    return false;
                }

                SelfCheckUnit.InitializeCheckBitMap(SelfCheckItemList, ConnectedChip.CheckBitMap, 0);

                InitializeProtection(ConnectedChip);
                InitializeDeviceStage(ConnectedChip);
                FirmwareOption.SM2ID = ConnectedChip.SM2ID;

                Visibility S03XVisible = ConnectedChip.IsSupportDeviceSN ? Visibility.Visible : Visibility.Collapsed;

                FlashFixCheckBox.Visibility = Visibility.Collapsed;

                DeviceSNBlock.Visibility = S03XVisible;
                DeviceSNShowBlock.Visibility = S03XVisible;
                DeviceTimeoutLable.Visibility = S03XVisible;
                DeviceTimeoutTextBox.Visibility = S03XVisible;

                if (Connector is USBConnector usbConnector && usbConnector.Mode == USBConnector.ModeEnum.MHDisk)
                {
                    Connector.SendCommand(Header.MSDHandshake2, null);
                }
                if (!ConnectedChip.IsInternalFlash)
                {
                    Connector.SendCommand(Header.FlashID, null);
                }
                Connector.SendReady();
                FirmwareOption.MaxTransferSize = Connector is USBConnector ? ConnectedChip.MaxMSDTransferSize : ConnectedChip.MaxSerialTransferSize; // Update MaxTransferSize
                Title = StaticTitle + " - " + ConnectedChip.Name;
            }
            else
            {
                Connector = null;
                DownloadButtonLabel.SetResourceReference(ContentProperty, "ui.Firmware.LoadButton.Load");
                UpdatePort(null, null);
                Title = StaticTitle;
                SessionTabControl.SelectedIndex = 0;
            }

            ChipConnectButtonLabel.SetResourceReference(ContentProperty, isConnected ? "ui.Connect.ConnectButton.Connected" : "ui.Connect.ConnectButton.Connect");
            PortGrid.IsEnabled = !isConnected;
            BaudrateGrid.IsEnabled = PortGrid.IsEnabled;

            if (isConnected && Connector.IsUserBootConnection)
            {
                ActionProgressBarShow(false);
                return true;
            }

            DeviceStageUpdateTab.Visibility = isConnected ? Visibility.Visible : Visibility.Collapsed;
            ChipInfoGroupBox.Visibility = isConnected ? Visibility.Visible : Visibility.Collapsed;
#if DEBUG
            SelfCheckTab.Visibility = isConnected ? Visibility.Visible : Visibility.Collapsed;
            FlashOptionTab.Visibility = isConnected && (!ConnectedChip.IsInternalFlash) ? Visibility.Visible : Visibility.Collapsed;
#else
            SelfCheckTab.Visibility = Visibility.Collapsed;
            FlashOptionTab.Visibility = isConnected && (!ConnectedChip.IsBondingFlash) && (!ConnectedChip.IsInternalFlash) ? Visibility.Visible : Visibility.Collapsed;
#endif
            SecurityTab.Visibility = ProtectGroupBox.Visibility = isConnected && ConnectedChip.IsSupportProtection ? Visibility.Visible : Visibility.Collapsed;


            ActionProgressBarShow(false);
            return true;
        }


        public void ConnectorCallBack(ConnectorArgs e)
        {
            Dispatcher.BeginInvoke(new Action(() =>
            {
                switch (e.Reason)
                {
#region 连接逻辑处理
                    case EventReason.Connecting:
                        ActionProgressBarChange(0, 1, (double)e.State);
                        break;
                    case EventReason.Connected:
                        if (e.Sender is SerialConnector) {
                            Connector = Serialcon;


                            Connector.SendCommand(Header.SerialHandshake1, null);
                            Connector.SendCommand(Header.SerialHandshake2, null);
                            Connector.SendReady();
                        }
                        else
                        {
                            Connector = MSDConnector;
                            Command msdHandshakeCommand = Connector.SendCommand(Header.MSDHandshake1, null);
                            msdHandshakeCommand.State = MSDConnector.Mode == USBConnector.ModeEnum.MHDisk ? TaskState.Read : TaskState.Write;
                            Connector.SendReady();
                        }
                        GlobalConfig.Set("Port", PortList.Text);
                        break;
                    case EventReason.ChipConnected:
                        if (!ChipConnected(true))
                        {
                            Connector.Disconnect();
                            if (AutoTaskOptions.IsRunning) AutoTaskOptions.AutoCurrent.Failed();
                            return;
                        }
                        Logger.Info("Chip {0} connected!", ConnectedChip.Name);
                        if (AutoTaskOptions.IsRunning) AutoTaskOptions.AutoCurrent.Succeed();
                        break;
                    case EventReason.ConnectFailed:
                        ChipConnected(false);
                        string failedReason = "";
                        if (e.State is string)
                            failedReason = Logger.T((string)e.State);
                        Logger.Warn("Chip connect failed! {0}", failedReason);
                        if (AutoTaskOptions.IsRunning) AutoTaskOptions.AutoCurrent.Failed();
                        break;
                    case EventReason.Disconnected:
                        if(e.State is string)
                            Logger.Warn("An exception occurred during disconnection: {0}", (string)e.State);
                        if (ChipConnectButtonLabel.Content == Lang["ui.Connect.ConnectButton.Connecting"])
                            Logger.Info("Stop connect chip!");
                        else
                            Logger.Info("Chip disconnected!");
                        ChipConnected(false);
                        if (AutoTaskOptions.IsRunning) AutoTaskOptions.AutoCurrent.Failed();
                        break;
#endregion

                    case EventReason.GotFlashID:
                        RPacketFlashID rpfid = (RPacketFlashID)e.State;
                        FlashOptions.MCUFlashOption = null;
                        foreach (WPacketFlashOption flash in FlashOptions.FlashTable.Values)
                        {
                            if(rpfid.FlashManu == flash.FlashManuValue && rpfid.FlashID == flash.FlashIDValue)
                            {
                                FlashOptions.MCUFlashOption = flash;
                            }
                        }
                        if (FlashOptions.MCUFlashOption != null)
                        {
                            if (FlashList.Text != FlashOptions.MCUFlashOption.FlashName)
                            {
                                FlashList.Text = FlashOptions.MCUFlashOption.FlashName;
                            }
#if DEBUG
                            Logger.Info($"芯片连接Flash型号: {FlashOptions.MCUFlashOption.FlashName} 厂商编号: 0x{rpfid.FlashManu:X02} ID: 0x{rpfid.FlashID:X04} 容量: {FlashOptions.MCUFlashOption.FlashSize}Mbit 。");
#endif
                        }
                        else
                        {
                            FlashList.SelectedIndex = -1;
                            FlashAddButton_Click(null, null);
                            FlashOptions.CurrentFlashOption.FlashManuValue = rpfid.FlashManu;
                            FlashOptions.CurrentFlashOption.FlashIDValue = rpfid.FlashID;
                            Logger.Warn("Chip storage exception, storage matching code {0}{1}, please feedback to FAE engineer.", rpfid.FlashManu.ToString("X02"), rpfid.FlashID.ToString("X04"));
                        }
                        break;
                    case EventReason.CryptoChecked:
                        ushort checkResult = (ushort)e.State;
                        if (SelfCheckUnit.UpdateCheckMap(SelfCheckItemList, checkResult))
                        {
                            Logger.Info("All self check items passed the check!");
                        }
                        else
                        {
                            Logger.Error("Self check item failed!");
                        }
                        break;
                    case EventReason.StageChange:
                        if (ConnectedChip == null) return;
                        byte stage = (byte)e.State;
                        ConnectedChip.Stage = ConnectedChip.IsSupportChipStage ? (StageEnum)stage : (StageEnum)(stage > 1 ? (stage + 1) : stage);

                        string chipStage = (string)Lang["Stage." + ConnectedChip.Stage.ToString()];
                        if (ConnectedChip.Stage != StageEnum.Unknown)
                            Logger.Info("Current chip stage: {0}", chipStage);
                        ChipStageShowBlock.Text = chipStage;
                        DeviceStageUpdate.IsUpdateAllowed = ConnectedChip.Stage == StageEnum.Device;
                        break;
                    case EventReason.ChipUpdated:
                        {
                            Logger.Info("Device updated!");
                            DeviceStageUpdate.IsUpdating = false;
                            DeviceStageUpdate.SaveRecord(Connector.ChipSN, Connector.DeviceSN, true, null);
                            if (!string.IsNullOrEmpty(Connector.DeviceSN))
                                Logger.Info("Generated new SN.");
                        }
                        if (AutoTaskOptions.IsRunning) AutoTaskOptions.AutoCurrent.Succeed();
                        break;
                    case EventReason.ChipUpdateFailed:
                        string error = e.State as string;
                        {
                            Logger.Error("Device update failed! {0}", error);
                            DeviceStageUpdate.IsUpdating = false;
                            DeviceStageUpdate.SaveRecord(Connector.ChipSN, Connector.DeviceSN, false, error);
                        }
                        if (AutoTaskOptions.IsRunning) AutoTaskOptions.AutoCurrent.Failed();
                        break;
                    case EventReason.ChipSNChange:
                        string chipSN = Connector != null ? Connector?.ChipSN : "";
                        ChipSNShowBlock.Text = chipSN.Insert(Math.Min(16, chipSN.Length), " ");
                        break;
                    case EventReason.DeviceSNChange:
                        string deviceSN = Connector != null ? Connector.DeviceSN : "";
                        DeviceSNShowBlock.Text = deviceSN.Insert(Math.Min(16, deviceSN.Length), " ");
                        break;
                    case EventReason.HeaderAck:
                        Logger.Info("Firmware header send succeeded!");
                        ActionProgressBarChange(0, 0.05, 1);
                        break;
                    case EventReason.HeaderNack:
                        Logger.Error("Firmware header send failed, {0}!", (string)e.State);
                        Connector.Disconnect();
                        break;
                    case EventReason.Erasering:
                        ActionProgressBarChange(0.05, 0.2, (double)e.State, "Erasing {0:0.00%}");
                        break;
                    case EventReason.EraserAck:
                        Logger.Info("Flash erase succeeded!");
                        ActionProgressBarChange(0.05, 0.2, 1, "Erasing {0:0.00%}");
                        break;
                    case EventReason.EraserNack:
                        Logger.Error("Flash erase failed, {0}!", (string)e.State);
                        Connector.Disconnect();
                        break;
                    case EventReason.DownloadAck:
                        WPacketDownload fdata = (WPacketDownload)e.State;
                        if (fdata.Address + fdata.Data.Length == FirmwareOption.Firmware.Length + FirmwareOption.Firmware.Address)
                        {
                            Logger.Info("Firmware loaded! End package length: {0}.", fdata.Data.Length);
                            Logger.Info("Download time: {0:0.000} s", (DateTime.Now - DownloadStartTime).TotalSeconds);
                            if (AutoTaskOptions.IsRunning) AutoTaskOptions.AutoCurrent.Succeed();
                            if (Connector.CanSendCommand)
                                Connector.Disconnect();
                            else
                                Logger.Info("Wait for boot ready...");
                        }
                        else
                        {
                            Logger.Info("Loading address: {0:X08}, Package length {1}.", fdata.Address, fdata.Data.Length);
                            ActionProgressBarChange(0.25, 0.75, (double)(fdata.Address + fdata.Data.Length - FirmwareOption.Firmware.Address) / FirmwareOption.Firmware.Length, "Loading {0:0.00%}");
                        }
                        break;
                    case EventReason.DownloadNack:
                        WPacketDownload fdatan = (WPacketDownload)e.State;
                        if (fdatan.Address + fdatan.Data.Length == FirmwareOption.Firmware.Length + FirmwareOption.Firmware.Address)
                        {
                            Logger.Error(
                                "Load failed! End package length: {0}, {1}!",
                                fdatan.Data.Length,
                                fdatan.NackCode != null ? string.Format(Logger.T("Error Code: {0}"), Hex.GetString(fdatan.NackCode)) : Logger.T("Timeout")
                                );
                        }
                        else
                        {
                            Logger.Error(
                                "Load interrupted! Interrupt address: {0:X08}, package length {1}, {2}!",
                                fdatan.Address,
                                fdatan.Data.Length,
                                fdatan.NackCode != null ? string.Format(Logger.T("Error Code: {0}"), Hex.GetString(fdatan.NackCode)) : Logger.T("Timeout")
                                );
                        }
                        Connector.Disconnect();
                        break;
                    case EventReason.VCPCompleted:
                        Connector.Disconnect();
                        break;
                    case EventReason.SerialChanged:
                        UpdatePort(this, null);
                        //if (AutoTaskOptions.IsRunning)
                        //{
                        //    AutoTaskOptions.AutoCurrent.Failed();
                        //}
                        break;
                    case EventReason.USBChanged:
                        UpdatePort(this, null);
                        break;
                }
            }));
        }
    }
}
