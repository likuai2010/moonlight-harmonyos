import {
  drawLine,
  pressedColor,
  getPercent,
  getHeight,
  getWidth,
  drawCircle,
  getDefaultColor,
  VirtualControllerElement
} from './common';
import { VirtualController } from './VirtualController';

interface DigitalButtonListener {
  /**
   * onClick event will be fired on button click.
   */
  onClick(): void;

  /**
   * onLongClick event will be fired on button long click.
   */
  onLongClick(): void;

  /**
   * onRelease event will be fired on button unpress.
   */
  onRelease(): void;
}

export class DigitalButton extends VirtualControllerElement {
  private listeners: DigitalButtonListener[] = [];
  layer: number

  constructor(controller: VirtualController, elementId: number, layer: number) {
    super(controller, elementId)
    this.layer = layer
  }

  addDigitalButtonListener(listener: DigitalButtonListener): void {
    this.listeners.push(listener);
  }

  onElementTouchEvent(event: TouchEvent) {
    switch (event.type) {
      case TouchType.Down: {
        this.pressed = true
        this.onClickCallback();
        break;
      }
      case TouchType.Move: {
        break;
      }
      case TouchType.Up: {
        this.pressed = false
        this.onReleaseCallback()
        break;
      }
    }
  }

  onElementDraw(canvas: CanvasRenderingContext2D) {
    canvas.clearRect(0, 0, this.context.width, this.context.height)
    canvas.fillStyle = "#00000000"
    //可以在这里绘制内容。
    const strokeWidth = 2
    this.context.beginPath();
    this.context.lineWidth = strokeWidth;
    this.context.strokeStyle = this.pressed ? pressedColor : getDefaultColor(this.virtualController)
    this.context.ellipse(getPercent(this.context.width, 50), getPercent(this.context.height, 50), getPercent(this.context.width, 50) - strokeWidth, getPercent(this.context.height, 50) - strokeWidth, 0, 0, Math.PI * 2);
    this.context.stroke()
    if (this.text) {
      const textSize = getPercent(this.context.width, 25)
      this.context.textAlign = "center"
      this.context.fillStyle = this.pressed ? pressedColor : getDefaultColor(this.virtualController)
      this.context.font = textSize + 'px sans-serif';
      this.context.fillText(this.text, getPercent(this.context.width, 50), getPercent(this.context.height, 60));
    }
  }

  text: string = "A"
  pressed: boolean = false

  private onClickCallback() {
    // notify listeners
    this.listeners.forEach(listener => listener.onClick());
  }

  private onLongClickCallback() {
    // notify listeners
    this.listeners.forEach(listener => listener.onLongClick());
  }

  private onReleaseCallback() {
    // notify listeners
    this.listeners.forEach(listener => listener.onRelease());
  }
}
