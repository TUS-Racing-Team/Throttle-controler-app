import { useState } from "react";
import Navbar from "./components/Navbar";
import StatusBar from "./components/StatusBar";
import Landing from "./pages/Landing";
import Dashboard from "./components/Dashboard";
import MapEditor from "./components/MapEditor";
import PIDPanel from "./components/PIDPanel";
import WarningsToast from "./components/WarningsToast";
import "./styles.css";

export default function App() {
  const [page, setPage] = useState("dashboard");
  const [connected, setConnected] = useState(false);
  const [warnings, setWarnings] = useState([]);

  let content;
  if (!connected) {
    content = <Landing onConnect={() => setConnected(true)} />;
  } else if (page === "dashboard") {
    content = <Dashboard setWarnings={setWarnings} />;
  } else if (page === "map") {
    content = <MapEditor />;
  } else if (page === "pid") {
    content = <PIDPanel />;
  }

  return (
    <div className="app-shell">
      <Navbar page={page} setPage={setPage} connected={connected} />
      <StatusBar connected={connected} />
      <main className="content">{content}</main>
      <WarningsToast warnings={warnings} />
    </div>
  );
}
