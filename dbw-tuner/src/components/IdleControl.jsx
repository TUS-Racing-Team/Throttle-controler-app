import { useState } from "react";
import Tooltip from "./Tooltip";
import "./card.css";

export default function IdleControl() {
  const [idle, setIdle] = useState(7.5);

  return (
    <div className="card">
      <h3>
        Idle Throttle
        <Tooltip text="Throttle opening used when the engine is idling and the pedal is released." />
      </h3>

      <p className="hint">
        This value defines how much the throttle is opened when the engine is at
        idle (no pedal input). Typical values are between 7.5% and 8.0%.
      </p>

      <div className="idle-row">
        <input
          type="range"
          min="5"
          max="12"
          step="0.1"
          value={idle}
          onChange={e => setIdle(Number(e.target.value))}
        />
        <input
          type="number"
          min="5"
          max="12"
          step="0.1"
          value={idle}
          onChange={e => setIdle(Number(e.target.value))}
        />
        <span>%</span>
      </div>

      <p className="hint">
        Increase this value if the engine stalls at idle. Decrease it if idle
        speed is too high.
      </p>

      <button className="btn accent">
        Send Idle Setting to Controller
      </button>
    </div>
  );
}
