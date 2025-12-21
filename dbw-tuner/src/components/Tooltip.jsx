export default function Tooltip({ text }) {
  return (
    <span className="tooltip-wrapper">
      ⓘ
      <span className="tooltip">{text}</span>
    </span>
  );
}
