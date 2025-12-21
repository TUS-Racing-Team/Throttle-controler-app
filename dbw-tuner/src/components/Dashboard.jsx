import { useEffect, useState, useRef } from "react";
import { startTelemetry } from "../services/telemetryMock";
import Tooltip from "./Tooltip";
import ThrottleCurve from "./ThrottleCurve";
import IdleControl from "./IdleControl";
import IdleRPMTarget from "./IdleRPMTarget";
import IdleCoolantTable from "./IdleCoolantTable";
import DebugConsole from "./DebugConsole";
import "./card.css";

export default function Dashboard({ setWarnings }) {
  const [t, setT] = useState({
    rpm: 0,
    pedal: 0,
    target: 0,
    actual: 0,
    canOk: true
  });

  const lastWarningsRef = useRef([]);

  useEffect(() => {
    startTelemetry(data => {
      setT(data);

      const currentWarnings = [];

      if (!data.canOk) currentWarnings.push("CAN timeout");
      if (Math.abs(data.actual - data.target) > 5)
        currentWarnings.push("Throttle deviation");

      if (
        JSON.stringify(currentWarnings) !==
        JSON.stringify(lastWarningsRef.current)
      ) {
        lastWarningsRef.current = currentWarnings;
        setWarnings(currentWarnings);
      }
    });
  }, []);

  return (
    <>
      {/* ===== LIVE TELEMETRY ===== */}
      <div className="card">
        <h3>Live Telemetry</h3>

        <div className="grid">
          <Gauge
            label="RPM"
            value={t.rpm}
            unit="rpm"
            help="Engine speed reported by the ECU"
          />
          <Gauge
            label="Pedal"
            value={t.pedal}
            unit="%"
            help="Driver pedal input position"
          />
          <Gauge
            label="Target"
            value={t.target}
            unit="%"
            help="Requested throttle opening after mapping"
          />
          <Gauge
            label="Actual"
            value={t.actual}
            unit="%"
            help="Actual throttle position measured by sensor"
          />
        </div>

        <ThrottleCurve
          data={[
            { rpm: 800, throttle: 7.5 },
            { rpm: 1500, throttle: 15 },
            { rpm: 3000, throttle: 40 },
            { rpm: 6000, throttle: 90 }
          ]}
        />
      </div>

      {/* ===== IDLE CONTROL ===== */}
      <IdleControl />
      <IdleRPMTarget />
      <IdleCoolantTable />

      {/* ===== DEBUG CONSOLE ===== */}
      <DebugConsole />
    </>
  );
}

function Gauge({ label, value, unit, help }) {
  return (
    <div className="gauge">
      <span className="label">
        {label}
        <Tooltip text={help} />
      </span>
      <span className="value">{value}</span>
      <span className="unit">{unit}</span>
    </div>
  );
}
