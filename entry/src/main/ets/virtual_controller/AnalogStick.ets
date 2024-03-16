import Prompt from '@system.prompt';
import {
  pressedColor,
  getPercent,
  getHeight,
  getWidth,
  drawCircle,
  getCorrectWidth,
  getDefaultColor,
  VirtualControllerElement,
} from './common';


export function getMovementRadius(x:number, y:number): number {
  return Math.sqrt(x * x + y * y);
}

export function getAngle(way_x:number, way_y:number): number {
  // prevent divisions by zero for corner cases
  if (way_x == 0) {
    return way_y < 0 ? Math.PI : 0;
  } else if (way_y == 0) {
    if (way_x > 0) {
      return Math.PI * 3 / 2;
    } else if (way_x < 0) {
      return Math.PI * 1 / 2;
    }
  }
  // return correct calculated angle for each quadrant
  if (way_x > 0) {
    if (way_y < 0) {
      // first quadrant
      return 3 * Math.PI / 2 + Math.atan((-way_y / way_x));
    } else {
      // second quadrant
      return Math.PI + Math.atan((way_x / way_y));
    }
  } else {
    if (way_y > 0) {
      // third quadrant
      return Math.PI / 2 + Math.atan((way_y / -way_x));
    } else {
      // fourth quadrant
      return 0 + Math.atan((-way_x / -way_y));
    }
  }
}

enum STICK_STATE {
  NO_MOVEMENT,
  MOVED_IN_DEAD_ZONE,
  MOVED_ACTIVE
}

enum CLICK_STATE {
  SINGLE,
  DOUBLE
}
export interface AnalogStickListener {
  /**
   * onMovement event will be fired on real analog stick movement (outside of the deadzone).
   *
   * @param x horizontal position, value from -1.0 ... 0 .. 1.0
   * @param y vertical position, value from -1.0 ... 0 .. 1.0
   */
  onMovement: (x: number, y: number)=> void;

  /**
   * onClick event will be fired on click on the analog stick
   */
  onClick: ()=>void;

  /**
   * onDoubleClick event will be fired on a double click in a short time frame on the analog
   * stick.
   */
  onDoubleClick:()=> void;

  /**
   * onRevoke event will be fired on unpress of the analog stick.
   */
  onRevoke:()=> void;
}



export class AnalogStick extends VirtualControllerElement {
  pressed = false
  timeoutDoubleClick = 500
  timeoutDeadzone = 150
  stick_state = STICK_STATE.NO_MOVEMENT;
  click_state = CLICK_STATE.SINGLE
  timeLastClick: number = 0
  radius_complete: number = 40
  radius_dead_zone: number = 30
  radius_analog_stick: number = 30
  position_stick_x: number = 0
  position_stick_y: number = 0
  relative_x: number = 0
  relative_y: number = 0
  movement_radius: number = 0
  movement_angle: number = 0
  private listeners: AnalogStickListener[] = [];
  addAnalogStickListener(listener: AnalogStickListener): void {
    this.listeners.push(listener);
  }
  private notifyOnMovement(x: number, y: number) {
    // notify listeners
    this.listeners.forEach(listener => listener.onMovement(x, y));
  }

  private notifyOnClick() {
    // notify listeners
    this.listeners.forEach(listener => listener.onClick());
  }

  private notifyOnDoubleClick() {
    // notify listeners
    this.listeners.forEach(listener => listener.onDoubleClick());
  }

  private notifyOnRevoke() {
    // notify listeners
    this.listeners.forEach(listener => listener.onRevoke());
  }

  onElementDraw(canvas: CanvasRenderingContext2D) {
    this.context.fillStyle = "#00000000"
    this.context.clearRect(0, 0, this.context.width, this.context.height)
    //可以在这里绘制内容。
    this.context.lineWidth = 3;
    const x = getPercent(this.context.width, 50)
    const y = getPercent(this.context.height, 50)
    this.context.strokeStyle = this.pressed ? pressedColor : getDefaultColor(this.virtualController)
    drawCircle(this.context, x, y, this.radius_complete)

    this.context.strokeStyle = getDefaultColor(this.virtualController)
    drawCircle(this.context, x, y, this.radius_dead_zone)

    switch (this.stick_state) {
      case STICK_STATE.NO_MOVEMENT: {
        this.context.strokeStyle = getDefaultColor(this.virtualController)
        drawCircle(this.context, getPercent(this.context.width, 50), getPercent(this.context.height, 50), this.radius_analog_stick)
        break;
      }
      case STICK_STATE.MOVED_IN_DEAD_ZONE:
      case STICK_STATE.MOVED_ACTIVE: {
        this.context.strokeStyle = pressedColor
        drawCircle(this.context, this.position_stick_x, this.position_stick_y, this.radius_analog_stick);
        break;
      }
    }
  }
  onElementTouchEvent(event: TouchEvent): boolean {
    let lastClickState = this.click_state;
    let x = event.changedTouches[0].x
    let y = event.changedTouches[0].y
    this.relative_x = -(getWidth(this.context) / 2 - x);
    this.relative_y = -(getHeight(this.context) / 2 - y);
    this.movement_radius = getMovementRadius(this.relative_x, this.relative_y);
    this.movement_angle = getAngle(this.relative_x, this.relative_y);

    // pass touch event to parent if out of outer circle
    if (this.movement_radius > this.radius_complete && !this.pressed)
      return false;

    // chop radius if out of outer circle or near the edge
    if (this.movement_radius > (this.radius_complete - this.radius_analog_stick)) {
      this.movement_radius = this.radius_complete - this.radius_analog_stick;
    }
    // handle event depending on action
    switch (event.type) {
      case TouchType.Down: {
        // set to dead zoned, will be corrected in update position if necessary
        this.stick_state = STICK_STATE.MOVED_IN_DEAD_ZONE;
        // check for double click
        if (lastClickState == CLICK_STATE.SINGLE &&
        event.timestamp - this.timeLastClick <= this.timeoutDoubleClick) {
          this.click_state = CLICK_STATE.DOUBLE;
          Prompt.showToast({ message: "DOUBLE" })
          this.notifyOnDoubleClick();
        } else {
          this.click_state = CLICK_STATE.SINGLE;
          //Prompt.showToast({ message: "SINGLE" })
          this.notifyOnClick();
        }
        // reset last click timestamp
        this.timeLastClick = event.timestamp;
        // set item pressed and update
        this.pressed = true
        break;
      }
    // up event (revoke touch)
      case TouchType.Cancel:
      case TouchType.Up: {
        this.pressed = false
        break;
      }
    }
    if (this.pressed) {
      // when is pressed calculate new positions (will trigger movement if necessary)
      this.updatePosition(event.timestamp);
    } else {
      this.stick_state = STICK_STATE.NO_MOVEMENT;
      this.notifyOnRevoke();

      // not longer pressed reset analog stick
      this.notifyOnMovement(0, 0);
    }
    return true
  }

  updatePosition(eventTime: number) {
    // get 100% way
    let complete = this.radius_complete - this.radius_analog_stick;

    // calculate relative way
    let correlated_y = (Math.sin(Math.PI / 2 - this.movement_angle) * (this.movement_radius));
    let correlated_x = (Math.cos(Math.PI / 2 - this.movement_angle) * (this.movement_radius));

    // update positions
    this.position_stick_x = getWidth(this.context) / 2 - correlated_x;
    this.position_stick_y = getHeight(this.context) / 2 - correlated_y;

    // Stay active even if we're back in the deadzone because we know the user is actively
    // giving analog stick input and we don't want to snap back into the deadzone.
    // We also release the deadzone if the user keeps the stick pressed for a bit to allow
    // them to make precise movements.
    this.stick_state = (this.stick_state == STICK_STATE.MOVED_ACTIVE ||
    eventTime - this.timeLastClick > this.timeoutDeadzone ||
    this.movement_radius > this.radius_dead_zone) ?
    STICK_STATE.MOVED_ACTIVE : STICK_STATE.MOVED_IN_DEAD_ZONE;

    //  trigger move event if state active
    if (this.stick_state == STICK_STATE.MOVED_ACTIVE) {
      this.notifyOnMovement(-correlated_x / complete, correlated_y / complete);
    }
  }

  onSizeChanged(canvas: CanvasRenderingContext2D) {
    this.radius_complete = getPercent(getCorrectWidth(canvas) / 2, 100) - 2 * 2;
    this.radius_dead_zone = getPercent(getCorrectWidth(canvas) / 2, 30);
    this.radius_analog_stick = getPercent(getCorrectWidth(canvas) / 2, 20);
  }
}
