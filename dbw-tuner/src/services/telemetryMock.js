let rpm = 900;
let dir = 1;

export function startTelemetry(cb) {
  setInterval(() => {
    rpm += dir * 120;
    if (rpm > 6500 || rpm < 900) dir *= -1;

    cb({
      rpm,
      pedal: Math.min(100, Math.floor(rpm / 65)),
      target: Math.min(100, Math.floor(rpm / 70)),
      actual: Math.min(100, Math.floor(rpm / 75)),
      canOk: true
    });
  }, 100);
}
