import { useState } from "react";
import Tooltip from "./Tooltip";
import "./map.css";
import "./card.css";

export default function IdleCoolantTable() {
  const [table, setTable] = useState([
    { temp: -10, rpm: 1200 },
    { temp: 0, rpm: 1100 },
    { temp: 20, rpm: 1000 },
    { temp: 40, rpm: 950 },
    { temp: 60, rpm: 900 },
    { temp: 80, rpm: 850 }
  ]);

  const update = (i, field, value) => {
    const copy = [...table];
    copy[i] = { ...copy[i], [field]: Number(value) };
    setTable(copy);
  };

  return (
    <div className="card full">
      <h3>
        Idle vs Coolant Temperature
        <Tooltip text="Adjusts idle speed based on engine temperature." />
      </h3>

      <p className="hint">
        Cold engines require higher idle speed to prevent stalling.
        As the engine warms up, idle RPM is gradually reduced.
      </p>

      <table className="map">
        <thead>
          <tr>
            <th>Coolant °C</th>
            <th>Target RPM</th>
          </tr>
        </thead>
        <tbody>
          {table.map((row, i) => (
            <tr key={i}>
              <td>
                <input
                  type="number"
                  value={row.temp}
                  onChange={e => update(i, "temp", e.target.value)}
                />
              </td>
              <td>
                <input
                  type="number"
                  value={row.rpm}
                  onChange={e => update(i, "rpm", e.target.value)}
                />
              </td>
            </tr>
          ))}
        </tbody>
      </table>

      <p className="hint">
        Values are interpolated between points.
        Similar to OEM ECU idle warm-up strategies.
      </p>

      <button className="btn accent">
        Upload Coolant Idle Table
      </button>
    </div>
  );
}
