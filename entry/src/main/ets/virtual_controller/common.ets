import { ControllerMode, VirtualController } from './VirtualController';

export function getPercent(value: number, percent: number): number {
  return value / 100 * percent;
}

export function getCorrectWidth(context: CanvasRenderingContext2D): number {
  return context.width > context.height ? context.height : context.width;
}

export function drawCircle(context: CanvasRenderingContext2D, x: number, y: number, radius: number) {
  context.beginPath();
  context.arc(x, y, radius, 0, Math.PI * 2, true);
  context.stroke()
}

export function drawRect(context: CanvasRenderingContext2D, left: number, top: number, right: number, bottom: number, fill: boolean = false) {
  if (fill) {
    context.fillRect(left, top, right - left, bottom - top)
  } else {
    context.strokeRect(left, top, right - left, bottom - top)
  }
}


export function drawLine(context: CanvasRenderingContext2D, left: number, top: number, right: number, bottom: number) {
  context.beginPath();
  context.moveTo(left, top);
  context.lineTo(right, bottom);
  context.stroke();
}


export function getWidth(context: CanvasRenderingContext2D): number {
  return context.width
}

export function getHeight(context: CanvasRenderingContext2D): number {
  return context.height
}

const normalColor = "#F0888888";

export const pressedColor = "#F00000FF";

const configMoveColor = "#F0FF0000";
const configResizeColor = "#F0FF00FF";
const configSelectedColor = "#F000FF00";

export function getDefaultColor(virtualController: VirtualController) {
  if (virtualController.currentMode == ControllerMode.MoveButtons)
    return configMoveColor;
  else if (virtualController.currentMode == ControllerMode.ResizeButtons)
    return configResizeColor;
  else
    return normalColor;
}

enum Mode {
  Normal,
  Resize,
  Move
}

@Observed
export class ElementLayoutParam {
  x: number;
  y: number;
  width: number;
  height: number;

  constructor(x: number, y: number, width: number, height: number) {
    this.x = x
    this.y = y
    this.width = width
    this.height = height
  }
  update(target: ElementLayoutParam): ElementLayoutParam{
    this.x = target.x
    this.y = target.y
    this.width = target.width
    this.height = target.height
    return this
  }
}

export abstract class VirtualControllerElement {
  public static EID_DPAD = 1;
  public static EID_LT = 2;
  public static EID_RT = 3;
  public static EID_LB = 4;
  public static EID_RB = 5;
  public static EID_A = 6;
  public static EID_B = 7;
  public static EID_X = 8;
  public static EID_Y = 9;
  public static EID_BACK = 10;
  public static EID_START = 11;
  public static EID_LS = 12;
  public static EID_RS = 13;
  public static EID_LSB = 14;
  public static EID_RSB = 15;

  elementId: number
  virtualController: VirtualController
  context!: CanvasRenderingContext2D
  normalColor = "#F0888888";
  pressedColor = "#F00000FF";
  private configMoveColor = "#0xF0FF0000";
  private configResizeColor = "#0xF0FF00FF";
  private configSelectedColor = "#0xF000FF00";
  protected startSize_x!: number;
  protected startSize_y!: number;
  position_pressed_x: number = 0;
  position_pressed_y: number = 0;
  private currentMode = Mode.Normal;
  layout!: ElementLayoutParam

  constructor(controller: VirtualController, elementId: number) {
    this.virtualController = controller;
    this.elementId = elementId;
  }

  setLayout(layout: ElementLayoutParam) {
    this.layout = layout;
  }

  getWidth(): number {
    return this.context.width
  }

  getHeight(): number {
    return this.context.height
  }

  getDefaultStrokeWidth(): number {
    return 2
  }

  resizeElement = (pressed_x:number, pressed_y:number, width:number, height:number) => {
    const layoutParams = this.layout
    let newHeight = height + (this.startSize_y - pressed_y);
    let newWidth = width + (this.startSize_x - pressed_x);
    layoutParams.height = newHeight > 20 ? newHeight : 20;
    layoutParams.width = newWidth > 20 ? newWidth : 20;
  }
  moveElement = (pressed_x:number, pressed_y:number, x:number, y:number) => {
    const layoutParams = this.layout
    const dx = x - pressed_x;
    const dy = y - pressed_y;
    layoutParams.x += dx
    layoutParams.y += dy
    this.position_pressed_x += dx;
    this.position_pressed_y += dy;
  }

  abstract onElementDraw(canvas: CanvasRenderingContext2D);

  abstract onElementTouchEvent(event: TouchEvent)

  onDraw(canvas: CanvasRenderingContext2D) {
    this.context = canvas
    this.onElementDraw(canvas);
    if (this.currentMode != Mode.Normal) {
      canvas.strokeStyle = configSelectedColor
      const strokeWidth = this.getDefaultStrokeWidth()
      canvas.lineWidth = strokeWidth
      drawRect(canvas, strokeWidth, strokeWidth, getWidth(canvas) - strokeWidth, getHeight(canvas) - strokeWidth)
    }
  }

  actionEnableMove() {
    this.currentMode = Mode.Move;
  }

  actionEnableResize() {
    this.currentMode = Mode.Resize;
  }

  actionCancel() {
    this.currentMode = Mode.Normal;
    //invalidate();
  }

  getConfiguration():string {
    return JSON.stringify(this.layout)
  }
  loadConfiguration(configuration: string){
    const target =  JSON.parse(configuration) as ElementLayoutParam
    this.layout = new ElementLayoutParam(0,0,0,0).update(target)
  }

  onSizeChanged(canvas: CanvasRenderingContext2D) {
  }
  touchId: number = -1
  onTouchEvent(event: TouchEvent):boolean | undefined {
    if (this.virtualController.currentMode == ControllerMode.Active) {
      return this.onElementTouchEvent(event);
    }
    let touch: TouchObject | undefined
    if(this.touchId > -1){
      touch = event.changedTouches.find((t)=>t.id == this.touchId)
      if (touch == null)
        return;
    }
    switch (event.type) {
      case TouchType.Down: {
        if(this.touchId == -1){
          touch = event.changedTouches[0]
          this.touchId = touch.id
        }
        this.position_pressed_x = touch?.x ?? 0;
        this.position_pressed_y = touch?.y ?? 0;
        this.startSize_x = this.getWidth();
        this.startSize_y = this.getHeight();
        if (this.virtualController.currentMode == ControllerMode.MoveButtons)
          this.actionEnableMove();
        else if (this.virtualController.currentMode == ControllerMode.ResizeButtons)
          this.actionEnableResize();
        return true;
      }
      case TouchType.Move: {
        switch (this.currentMode) {
          case Mode.Move: {
            this.moveElement(
              this.position_pressed_x,
              this.position_pressed_y,
              touch?.x ?? 0,
              touch?.y ?? 0);
            break;
          }
          case Mode.Resize: {
            this.resizeElement(
              this.position_pressed_x,
              this.position_pressed_y,
              touch?.x ?? 0,
              touch?.y ?? 0);
            break;
          }
          case Mode.Normal: {
            break;
          }
        }
        return true;
      }
      case TouchType.Cancel:{}
      case TouchType.Up: {
        this.touchId = -1
        this.actionCancel();
        return true;
      }
      default: {
      }
    }
    return true;
  }
}

@Component
export struct VirtualControllerButton {

  //用来配置CanvasRenderingContext2D对象的参数，包括是否开启抗锯齿，true表明开启抗锯齿。
  private settings: RenderingContextSettings = new RenderingContextSettings(true)
  //用来创建CanvasRenderingContext2D对象，通过在canvas中调用CanvasRenderingContext2D对象来绘制。
  private context: CanvasRenderingContext2D = new CanvasRenderingContext2D(this.settings)
  element!: VirtualControllerElement
  @ObjectLink layout: ElementLayoutParam

  onLayout() {
    this.element.onSizeChanged(this.context)
  }

  build() {
    Canvas(this.context)
      .height(this.layout.height)
      .width(this.layout.width)
      .translate({ x: this.layout.x, y: this.layout.y })
      .opacity(1)
      .onReady(() => {
        this.element.onDraw(this.context)
      })
      .onTouch((e) => {
        // 单点
        this.element.onTouchEvent(e)
        this.element.onDraw(this.context)
      })
  }
}

