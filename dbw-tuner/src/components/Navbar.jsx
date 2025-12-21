export default function Navbar({ page, setPage, connected }) {
  if (!connected) return null;

  return (
    <nav className="navbar">
      <span className="logo">DBW Tuner</span>

      <div className="nav-buttons">
        <button
          className={page === "dashboard" ? "active" : ""}
          onClick={() => setPage("dashboard")}
        >
          Dashboard
        </button>
        <button
          className={page === "map" ? "active" : ""}
          onClick={() => setPage("map")}
        >
          Throttle Map
        </button>
        <button
          className={page === "pid" ? "active" : ""}
          onClick={() => setPage("pid")}
        >
          PID
        </button>
      </div>
    </nav>
  );
}
