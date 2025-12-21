import { useState } from "react";
import { heatColor } from "../utils/heatColor";
import { presets } from "../presets";
import Tooltip from "./Tooltip";
import "./map.css";
import "./card.css";

export default function MapEditor() {
  const [map, setMap] = useState(
    Array.from({ length: 8 }, () =>
      Array.from({ length: 8 }, (_, j) => j * 12)
    )
  );

  const update = (r, c, v) => {
    const copy = map.map(row => [...row]);
    copy[r][c] = Number(v);
    setMap(copy);
  };

  const applyPreset = name => {
    setMap(
      Array.from({ length: 8 }, (_, r) =>
        Array.from({ length: 8 }, (_, c) =>
          Math.min(100, presets[name](r, c))
        )
      )
    );
  };

  return (
    <div className="card full">
      <h3>
        Throttle Mapping
        <Tooltip text="Defines how pedal input is translated into throttle opening." />
      </h3>

      <p className="hint">
        Rows represent engine speed (RPM). Columns represent pedal position.
        Each cell defines how much the throttle opens for that condition.
      </p>

      <div style={{ marginBottom: 10 }}>
        {Object.keys(presets).map(p => (
          <button
            key={p}
            className="btn"
            title={
              p === "Eco"
                ? "Smooth and less aggressive throttle response"
                : p === "Sport"
                ? "Balanced throttle response for daily driving"
                : "Direct and aggressive throttle response"
            }
            onClick={() => applyPreset(p)}
          >
            {p}
          </button>
        ))}
      </div>

      <table className="map">
        <tbody>
          {map.map((row, r) => (
            <tr key={r}>
              {row.map((val, c) => (
                <td key={c}>
                  <input
                    type="number"
                    min="0"
                    max="100"
                    value={val}
                    style={{
                      background: heatColor(val),
                      color: "#000",
                      fontWeight: "bold"
                    }}
                    onChange={e => update(r, c, e.target.value)}
                  />
                </td>
              ))}
            </tr>
          ))}
        </tbody>
      </table>

      <p className="hint">
        Warmer colors indicate a more aggressive throttle response.
        Values are percentages of throttle opening.
      </p>

      <button className="btn accent">Upload Map to Controller</button>
    </div>
  );
}
