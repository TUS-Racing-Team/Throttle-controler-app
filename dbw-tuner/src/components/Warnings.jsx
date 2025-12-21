export default function Warnings({ flags }) {
  if (!flags.length) return null;

  return (
    <div className="card warning">
      <h3>⚠ SYSTEM WARNINGS</h3>
      <ul>
        {flags.map((f, i) => (
          <li key={i}>{f}</li>
        ))}
      </ul>
    </div>
  );
}
