import { useEffect, useState } from "react";

const MIN_DISPLAY_MS = 5000; // 👈 минимум 5 секунди

export default function WarningsToast({ warnings }) {
  const [visibleWarnings, setVisibleWarnings] = useState([]);
  const [timeoutId, setTimeoutId] = useState(null);

  useEffect(() => {
    if (warnings.length) {
      // показваме warnings
      setVisibleWarnings(warnings);

      // ако има стар таймер – махаме го
      if (timeoutId) clearTimeout(timeoutId);

      // слагаме нов таймер
      const id = setTimeout(() => {
        setVisibleWarnings([]);
      }, MIN_DISPLAY_MS);

      setTimeoutId(id);
    }
  }, [warnings]);

  if (!visibleWarnings.length) return null;

  return (
    <div className="warnings-toast">
      {visibleWarnings.map((w, i) => (
        <div key={i} className="toast">
          ⚠ {w}
          <div className="hint">
            {w === "CAN timeout" &&
              "No data received from ECU. Throttle may enter safe mode."}
            {w === "Throttle deviation" &&
              "Actual throttle position does not match the requested value."}
          </div>
        </div>
      ))}
    </div>
  );
}
