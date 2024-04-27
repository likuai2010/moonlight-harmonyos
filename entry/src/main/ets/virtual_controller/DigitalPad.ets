import {
  drawLine,
  drawRect,
  pressedColor,
  getPercent,
  getHeight,
  getWidth,
  getDefaultColor,
  VirtualControllerElement
} from './common';

const DIGITAL_PAD_DIRECTION_NO_DIRECTION = 0;
export const DIGITAL_PAD_DIRECTION_LEFT: number = 1;
export const DIGITAL_PAD_DIRECTION_UP: number = 2;
export const DIGITAL_PAD_DIRECTION_RIGHT: number = 4;
export const DIGITAL_PAD_DIRECTION_DOWN: number = 8;
interface DigitalPadListener {
  onDirectionChange: (direction:number) => void;
}

export class DigitalPad extends VirtualControllerElement {
  onSizeChanged(canvas: CanvasRenderingContext2D) {
  }

  pressed: boolean = false
  padDirection: number = DIGITAL_PAD_DIRECTION_NO_DIRECTION
  DPAD_MARGIN: number = 5;
  private listeners: DigitalPadListener[] = [];
  addDigitalPadListener(listener: DigitalPadListener): void {
    this.listeners.push(listener);
  }
  private newDirectionCallback(direction: number) {
    // notify listeners
    this.listeners.forEach(listener => listener.onDirectionChange(direction));
  }
  onElementTouchEvent(event: TouchEvent) {
    switch (event.type) {
      case TouchType.Down: {
        break;
      }
      case TouchType.Move: {
        let direction = 0;
        let x = event.changedTouches[0].x
        let y = event.changedTouches[0].y
        if (x < getPercent(getWidth(this.context), 33)) {
          direction |= DIGITAL_PAD_DIRECTION_LEFT;
        }
        if (x > getPercent(getWidth(this.context), 66)) {
          direction |= DIGITAL_PAD_DIRECTION_RIGHT;
        }
        if (y > getPercent(getHeight(this.context), 66)) {
          direction |= DIGITAL_PAD_DIRECTION_DOWN;
        }
        if (y < getPercent(getHeight(this.context), 33)) {
          direction |= DIGITAL_PAD_DIRECTION_UP;
        }
        this.padDirection = direction
        this.newDirectionCallback(direction);
        break;
      }
      case TouchType.Cancel:
      case TouchType.Up: {
        this.padDirection = 0;
        this.newDirectionCallback(this.padDirection);
        //invalidate();
        break;
      }
    }
  }

  onElementDraw(canvas: CanvasRenderingContext2D) {
    this.context.clearRect(0, 0, this.context.width, this.context.height)
    const strokeWidth = 2
    this.context.fillStyle = "#00000000"
    this.context.lineWidth = 2;
    if (this.padDirection == DIGITAL_PAD_DIRECTION_NO_DIRECTION) {
      // draw no direction rect
      this.context.strokeStyle = getDefaultColor(this.virtualController)
      drawRect(
        this.context,
        getPercent(getWidth(this.context), 36), getPercent(getHeight(this.context), 36),
        getPercent(getWidth(this.context), 63), getPercent(getHeight(this.context), 63),
      );
    }
    this.context.strokeStyle = (this.padDirection & DIGITAL_PAD_DIRECTION_LEFT) > 0 ? pressedColor : getDefaultColor(this.virtualController)
    drawRect(
      this.context,
      strokeWidth + this.DPAD_MARGIN,
      getPercent(getHeight(this.context), 33),
      getPercent(getWidth(this.context), 33),
      getPercent(getHeight(this.context), 66)
    );
    this.context.strokeStyle = (this.padDirection & DIGITAL_PAD_DIRECTION_UP) > 0 ? pressedColor : getDefaultColor(this.virtualController)
    drawRect(
      this.context,
      getPercent(getWidth(this.context), 33), strokeWidth + this.DPAD_MARGIN,
      getPercent(getWidth(this.context), 66), getPercent(getHeight(this.context), 33),
    );
    this.context.strokeStyle = (this.padDirection & DIGITAL_PAD_DIRECTION_RIGHT) > 0 ? pressedColor : getDefaultColor(this.virtualController)
    drawRect(
      this.context,
      getPercent(getWidth(this.context), 66), getPercent(getHeight(this.context), 33),
      getWidth(this.context) - (strokeWidth + this.DPAD_MARGIN),
      getPercent(getHeight(this.context), 66),
    );
    this.context.strokeStyle = (this.padDirection & DIGITAL_PAD_DIRECTION_DOWN) > 0 ? pressedColor : getDefaultColor(this.virtualController)
    drawRect(
      this.context,
      getPercent(getWidth(this.context), 33), getPercent(getHeight(this.context), 66),
      getPercent(getWidth(this.context), 66), getHeight(this.context) - (strokeWidth + this.DPAD_MARGIN),
    );

    this.context.strokeStyle = ((this.padDirection & DIGITAL_PAD_DIRECTION_LEFT) > 0 &&
    (this.padDirection & DIGITAL_PAD_DIRECTION_UP) > 0) ? pressedColor : getDefaultColor(this.virtualController)
    drawLine(this.context,
      strokeWidth + this.DPAD_MARGIN,
      getPercent(getHeight(this.context), 33),
      getPercent(getWidth(this.context), 33),
      strokeWidth + this.DPAD_MARGIN,
    )

    this.context.strokeStyle = ((this.padDirection & DIGITAL_PAD_DIRECTION_UP) > 0 &&
    (this.padDirection & DIGITAL_PAD_DIRECTION_RIGHT) > 0) ? pressedColor : getDefaultColor(this.virtualController)
    drawLine(this.context,
      getPercent(getWidth(this.context), 66),
      strokeWidth + this.DPAD_MARGIN,
      getWidth(this.context) - (strokeWidth + this.DPAD_MARGIN),
      getPercent(getHeight(this.context), 33)
    )
    this.context.strokeStyle = ((this.padDirection & DIGITAL_PAD_DIRECTION_RIGHT) > 0 &&
    (this.padDirection & DIGITAL_PAD_DIRECTION_DOWN) > 0) ? pressedColor : getDefaultColor(this.virtualController)
    drawLine(this.context,
      getWidth(this.context) - (strokeWidth + this.DPAD_MARGIN),
      getPercent(getHeight(this.context), 66),
      getPercent(getWidth(this.context), 66),
      getHeight(this.context) - (strokeWidth + this.DPAD_MARGIN),
    )
    this.context.strokeStyle = ((this.padDirection & DIGITAL_PAD_DIRECTION_DOWN) > 0 &&
    (this.padDirection & DIGITAL_PAD_DIRECTION_LEFT) > 0) ? pressedColor : getDefaultColor(this.virtualController)
    drawLine(this.context,
      getPercent(getWidth(this.context), 33),
      getHeight(this.context) - (strokeWidth + this.DPAD_MARGIN),
      strokeWidth + this.DPAD_MARGIN,
      getPercent(getHeight(this.context), 66),
    )
  }
}
