export default function Landing({ onConnect }) {
  return (
    <div className="landing">
      <h1>DBW Throttle Tuner</h1>

      <p className="hint">
        This application allows you to configure and tune an electronic
        drive-by-wire throttle controller.
      </p>

      <button className="btn accent big" onClick={onConnect}>
        Connect to Arduino
      </button>

      <p className="hint">
        (Currently running in mock mode – real Serial connection later)
      </p>
    </div>
  );
}
