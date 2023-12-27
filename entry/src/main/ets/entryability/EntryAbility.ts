import UIAbility from '@ohos.app.ability.UIAbility';
import hilog from '@ohos.hilog';
import window from '@ohos.window';
import { LimelightCertProvider } from './crypto/LimelightCryptoProvider';
import fileIo from '@ohos.file.fs';
import inputDevice from '@ohos.multimodalInput.inputDevice';
import InputEvent from '@ohos.multimodalInput.inputEvent';


export default class EntryAbility extends UIAbility {
  onCreate(want, launchParam) {
    hilog.info(0x0000, 'testTag', '%{public}s', 'Ability onCreate');
  }

  onDestroy() {
    hilog.info(0x0000, 'testTag', '%{public}s', 'Ability onDestroy');
  }

  onWindowStageCreate(windowStage: window.WindowStage) {
    // Main window is created, set main page for this ability
    hilog.info(0x0000, 'testTag', '%{public}s', 'Ability onWindowStageCreate');
    this.context.resourceManager.getRawFd('client.pem').then((i)=>{
      fileIo.copyFile(i.fd, this.context.cacheDir + "/client.pem").then((d)=>{
        console.log(d+"");
      })
    })
    this.context.resourceManager.getRawFd('client.key').then((i)=>{
      fileIo.copyFile(i.fd, this.context.cacheDir + "/private.pem")
        .then((d)=>{
          console.log(d+"");
        })
    })

    hilog.info(0x0000, 'testTag', `filesDir ${this.context.filesDir}`);
    hilog.info(0x0000, 'testTag', `distributedFilesDir ${this.context.distributedFilesDir}`);
    this.context.resourceManager.getRawFd('video1_640_272.yuv').then((i)=>{
      fileIo.copyFile(i.fd, this.context.cacheDir + "/video1_640_272.yuv")
        .then((d)=>{
          console.log(d+ "");
        })
    })
    LimelightCertProvider.filesDir = this.context.cacheDir
    windowStage.loadContent('pages/Index', (err, data) => {
      if (err.code) {
        hilog.error(0x0000, 'testTag', 'Failed to load the content. Cause: %{public}s', JSON.stringify(err) ?? '');
        return;
      }
      hilog.info(0x0000, 'testTag', 'Succeeded in loading the content. Data: %{public}s', JSON.stringify(data) ?? '');
    });

    try {
      InputEvent
      inputDevice.getDeviceList((error, ids) => {
        ids.forEach((d)=>{
          inputDevice.getDeviceInfo(d, (error, deviceData) => {
            console.log(`Device info: ${JSON.stringify(deviceData)}`);
            if(deviceData.name == "BSP-D8 Consumer Control"){
              inputDevice.on("change",(d)=>{

              })
            }
          });
        })
        hilog.info(0x0000, 'testTag',`Device id list: ${JSON.stringify(ids)}`);
      });
    } catch (error) {
      hilog.info(0x0000, 'testTag',`Failed to get device id list, error: ${JSON.stringify(error, [`code`, `message`])}`);
    }
  }

  onWindowStageDestroy() {
    // Main window is destroyed, release UI related resources
    hilog.info(0x0000, 'testTag', '%{public}s', 'Ability onWindowStageDestroy');
  }

  onForeground() {
    // Ability has brought to foreground
    hilog.info(0x0000, 'testTag', '%{public}s', 'Ability onForeground');
  }

  onBackground() {
    // Ability has back to background
    hilog.info(0x0000, 'testTag', '%{public}s', 'Ability onBackground');
  }
};
