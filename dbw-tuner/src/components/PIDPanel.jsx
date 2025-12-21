import { useState } from "react";
import Tooltip from "./Tooltip";
import "./card.css";

export default function PIDPanel() {
  const [pid, setPid] = useState({
    kp: 4.0,
    ki: 0.1,
    kd: 0.0
  });

  return (
    <div className="card">
      <h3>PID Controller</h3>

      <p className="hint">
        The PID controller adjusts the throttle motor to match the requested
        throttle position smoothly and accurately.
      </p>

      <div className="pid-section">
        <label>
          P – Proportional
          <Tooltip text="Controls how strongly the throttle reacts to an error between target and actual position." />
        </label>
        <p className="pid-desc">
          Higher values make the throttle respond faster, but too high may cause
          oscillations.
        </p>
        <input
          type="range"
          min="0"
          max="10"
          step="0.01"
          value={pid.kp}
          onChange={e => setPid({ ...pid, kp: Number(e.target.value) })}
        />
        <span>{pid.kp.toFixed(2)}</span>
      </div>

      <div className="pid-section">
        <label>
          I – Integral
          <Tooltip text="Corrects long-term steady-state error." />
        </label>
        <p className="pid-desc">
          Helps the throttle reach the exact target position. Too much can cause
          slow response or overshoot.
        </p>
        <input
          type="range"
          min="0"
          max="2"
          step="0.01"
          value={pid.ki}
          onChange={e => setPid({ ...pid, ki: Number(e.target.value) })}
        />
        <span>{pid.ki.toFixed(2)}</span>
      </div>

      <div className="pid-section">
        <label>
          D – Derivative
          <Tooltip text="Dampens fast movements and reduces overshoot." />
        </label>
        <p className="pid-desc">
          Smooths throttle motion. Usually kept low or zero for throttle
          systems.
        </p>
        <input
          type="range"
          min="0"
          max="2"
          step="0.01"
          value={pid.kd}
          onChange={e => setPid({ ...pid, kd: Number(e.target.value) })}
        />
        <span>{pid.kd.toFixed(2)}</span>
      </div>

      <button className="btn accent">Send PID to Controller</button>
    </div>
  );
}
