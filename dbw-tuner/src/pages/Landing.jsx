import SerialConnect from "../components/SerialConnect";

export default function Landing({ onConnect }) {
  return (
    <div className="landing">
      <h1>DBW Throttle Tuner</h1>

      <p className="hint">
        Connect to the throttle controller to begin configuration.
      </p>

      <SerialConnect onConnected={onConnect} />
    </div>
  );
}
