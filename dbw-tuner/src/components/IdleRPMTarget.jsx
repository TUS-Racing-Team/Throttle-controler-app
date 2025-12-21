import { useState } from "react";
import Tooltip from "./Tooltip";
import "./card.css";

export default function IdleRPMTarget() {
  const [rpm, setRpm] = useState(900);

  return (
    <div className="card">
      <h3>
        Idle RPM Target
        <Tooltip text="Target engine speed when idling with no pedal input." />
      </h3>

      <p className="hint">
        This value defines the desired engine speed at idle.
        The controller will adjust throttle opening to maintain this RPM.
      </p>

      <div className="idle-row">
        <input
          type="range"
          min="600"
          max="1500"
          step="10"
          value={rpm}
          onChange={e => setRpm(Number(e.target.value))}
        />
        <input
          type="number"
          min="600"
          max="1500"
          step="10"
          value={rpm}
          onChange={e => setRpm(Number(e.target.value))}
        />
        <span>rpm</span>
      </div>

      <p className="hint">
        Typical values: 850–950 rpm for warm engines,
        higher values may be needed for aggressive cams.
      </p>

      <button className="btn accent">
        Send Idle RPM Target
      </button>
    </div>
  );
}
