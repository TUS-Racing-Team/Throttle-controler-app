export default function StatusBar({ connected }) {
  return (
    <div className={`statusbar ${connected ? "ok" : "bad"}`}>
      {connected
        ? "🟢 Connected to throttle controller"
        : "🔴 No controller detected. Please connect a device."}
    </div>
  );
}
