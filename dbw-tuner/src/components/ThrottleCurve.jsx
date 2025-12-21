import { useEffect, useRef } from "react";

export default function ThrottleCurve({ data }) {
  const canvasRef = useRef();

  useEffect(() => {
    const c = canvasRef.current;
    const ctx = c.getContext("2d");

    ctx.clearRect(0, 0, c.width, c.height);

    ctx.strokeStyle = "#00e0ff";
    ctx.lineWidth = 2;
    ctx.beginPath();

    data.forEach((p, i) => {
      const x = (p.rpm / 7000) * c.width;
      const y = c.height - (p.throttle / 100) * c.height;
      if (i === 0) ctx.moveTo(x, y);
      else ctx.lineTo(x, y);
    });

    ctx.stroke();
  }, [data]);

  return (
    <canvas
      ref={canvasRef}
      width={420}
      height={200}
      style={{
        background: "#0f1420",
        borderRadius: 8,
        marginTop: 12
      }}
    />
  );
}
