export function heatColor(value) {
  const r = Math.min(255, value * 2.5);
  const g = Math.min(255, 255 - value * 1.5);
  return `rgb(${r},${g},80)`;
}
