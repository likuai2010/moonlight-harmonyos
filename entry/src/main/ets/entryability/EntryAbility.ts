import UIAbility from '@ohos.app.ability.UIAbility';
import hilog from '@ohos.hilog';
import window from '@ohos.window';
import { LimelightCertProvider } from './crypto/LimelightCryptoProvider';
import fileIo from '@ohos.file.fs';
import inputDevice from '@ohos.multimodalInput.inputDevice';
import InputEvent from '@ohos.multimodalInput.inputEvent';

let windowStage_ = null;
let sub_windowClass = null;
export default class EntryAbility extends UIAbility {
  showSubWindow() {
    // 1.创建应用子窗口。
    windowStage_.createSubWindow("mySubWindow", (err, data) => {
      if (err.code) {
        return;
      }
      sub_windowClass = data;
      // 2.子窗口创建成功后，设置子窗口的位置、大小及相关属性等。
      sub_windowClass.moveWindowTo(300, 300);
      sub_windowClass.resize(500, 500, (err) => {
        if (err.code) {
          console.error('Failed to change the window size. Cause:' + JSON.stringify(err));
          return;
        }
        console.info('Succeeded in changing the window size.');
      });
      // 3.为子窗口加载对应的目标页面。
      sub_windowClass.setUIContent("pages/Loading", (err) => {
        if (err.code) {
          console.error('Failed to load the content. Cause:' + JSON.stringify(err));
          return;
        }
        // 3.显示子窗口。
        sub_windowClass.showWindow((err) => {
          if (err.code) {
            return;
          }
        });
      });
    })
  }

  onCreate(want, launchParam) {
    hilog.info(0x0000, 'testTag', '%{public}s', 'Ability onCreate');
  }
  onDestroy() {
    hilog.info(0x0000, 'testTag', '%{public}s', 'Ability onDestroy');
  }

  onWindowStageCreate(windowStage: window.WindowStage) {
    // Main window is created, set main page for this ability
    hilog.info(0x0000, 'testTag', '%{public}s', 'Ability onWindowStageCreate');
    let names = [];
    windowStage.getMainWindowSync().setWindowSystemBarEnable(names)

    windowStage_ = windowStage;
    // 开发者可以在适当的时机，如主窗口上按钮点击事件等，创建子窗口。并不一定需要在onWindowStageCreate调用，这里仅作展示


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
