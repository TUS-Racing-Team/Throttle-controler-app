import { useEffect, useRef, useState } from "react";
import { onSerialLog } from "../services/serialService";
import "./debug.css";

export default function DebugConsole() {
  const [logs, setLogs] = useState([]);
  const bottomRef = useRef();

  useEffect(() => {
    onSerialLog(log => {
      setLogs(prev => [...prev.slice(-300), log]);
    });
  }, []);

//   useEffect(() => {
//     bottomRef.current?.scrollIntoView({ behavior: "smooth" });
//   }, [logs]);

  return (
    <div className="debug">
      <h3>Debug Console</h3>

      <div className="log-window">
        {logs.map((l, i) => (
          <div key={i} className={`log ${l.level}`}>
            [{l.level.toUpperCase()}] {l.message}
          </div>
        ))}
        <div ref={bottomRef} />
      </div>
    </div>
  );
}
