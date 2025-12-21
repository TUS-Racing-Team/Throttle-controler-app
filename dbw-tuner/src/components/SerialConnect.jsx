import { useEffect, useState } from "react";
import {
  listPorts,
  connectPort,
  disconnectPort
} from "../services/serialService";
import "./card.css";

export default function SerialConnect({ onConnected }) {
  const [ports, setPorts] = useState([]);
  const [selected, setSelected] = useState("");
  const [connected, setConnected] = useState(false);
  const [error, setError] = useState("");

  async function refresh() {
    const list = await listPorts();
    setPorts(list);
  }

  async function connect() {
    try {
      await connectPort(selected);
      setConnected(true);
      setError("");
      onConnected(true);
    } catch (e) {
      setError(e);
    }
  }

  async function disconnect() {
    await disconnectPort();
    setConnected(false);
    onConnected(false);
  }

  useEffect(() => {
    refresh();
  }, []);

  return (
    <div className="card">
      <h3>Device Connection</h3>

      <select
        value={selected}
        onChange={e => setSelected(e.target.value)}
        disabled={connected}
      >
        <option value="">Select COM port</option>
        {ports.map(p => (
          <option key={p.path} value={p.path}>
            {p.path} {p.manufacturer && `(${p.manufacturer})`}
          </option>
        ))}
      </select>

      <div style={{ marginTop: 10 }}>
        {!connected ? (
          <button
            className="btn accent"
            disabled={!selected}
            onClick={connect}
          >
            Connect
          </button>
        ) : (
          <button className="btn" onClick={disconnect}>
            Disconnect
          </button>
        )}
      </div>

      {error && <p style={{ color: "#ff4d4d" }}>{error}</p>}
    </div>
  );
}
