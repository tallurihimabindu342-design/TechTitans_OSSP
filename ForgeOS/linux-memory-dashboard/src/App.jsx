import { useEffect, useState } from "react";

import {
  BarChart,
  Bar,
  LineChart,
  Line,
  XAxis,
  YAxis,
  CartesianGrid,
  Tooltip,
  ResponsiveContainer,
  Legend,
} from "recharts";

import "./App.css";


function App() {

  /* =====================================================
     BACKEND CONNECTION
     ===================================================== */

  const [memoryData, setMemoryData] = useState(null);
  const [connected, setConnected] = useState(false);

  const [memoryTrend, setMemoryTrend] = useState([]);

  const [systemMemory, setSystemMemory] = useState(null);


  useEffect(() => {

    const fetchMemoryData = async () => {

      try {

        const response =
          await fetch("http://localhost:5000/api/memory");

        if (!response.ok) {
          throw new Error("Backend response failed");
        }

        const data = await response.json();

        setMemoryData(data);
        setConnected(data.connected === true);


        /* ================================================
           LINUX /proc/meminfo (separate endpoint)
           ================================================ */

        try {
          const sysResponse =
            await fetch("http://localhost:5000/api/system-memory");

          const sysData = await sysResponse.json();

          setSystemMemory(
            sysData.available ? sysData : null
          );
        } catch {
          setSystemMemory(null);
        }

        if (data.connected !== true) {
          return;
        }


        /* ================================================
           UPDATE UTILIZATION TREND
           ================================================ */

        const now = new Date();

        const time =
          now.toLocaleTimeString([], {
            hour: "2-digit",
            minute: "2-digit",
            second: "2-digit",
          });

        setMemoryTrend((previous) => {

          const updated = [
            ...previous,
            {
              time,
              utilization: Number(data.utilization || 0),
            },
          ];

          /*
           * Keep only the latest 7 points
           */
          return updated.slice(-7);

        });


      } catch (error) {

        console.error(
          "Backend connection error:",
          error
        );

        setConnected(false);

      }

    };


    /*
     * First request immediately
     */
    fetchMemoryData();


    /*
     * Refresh every 2 seconds
     */
    const interval =
      setInterval(fetchMemoryData, 2000);


    return () => clearInterval(interval);

  }, []);


  /* =====================================================
     LOADING SCREEN
     ===================================================== */

  if (!memoryData) {

    return (
      <div className="app">

        <div
          style={{
            padding: "40px",
            color: "white",
            textAlign: "center",
          }}
        >

          <h2>
            Connecting to Linux memory backend...
          </h2>

          <p>
            Waiting for C allocator data.
          </p>

        </div>

      </div>
    );

  }


  /* =====================================================
     REAL DATA FROM C BACKEND
     ===================================================== */

  const totalMemory =
    Number(memoryData.totalMemory || 0);

  const assignedMemory =
    Number(memoryData.usedMemory || 0);

  const freeMemory =
    Number(memoryData.freeMemory || 0);

  const utilization =
    Number(memoryData.utilization || 0);

  const activeAllocations =
    Number(memoryData.activeAllocations || 0);

  const freeBlocks =
    Number(memoryData.freeBlocks || 0);

  const largestFree =
    Number(memoryData.largestFree || 0);

  const internalFragmentation =
    Number(memoryData.internalFragmentation || 0);

  const externalFragmentation =
    Number(memoryData.externalFragmentation || 0);

  const strategy =
    memoryData.strategy || "First Fit";

  const allocationMode =
    memoryData.allocationMode || "Contiguous";


  /* =====================================================
     MEMORY BLOCKS FROM C
     ===================================================== */

  /*
   * Non-contiguous allocations own several physical segments
   * sharing one allocationId, but requestedSize is stored only
   * on the first segment. Total the segments per allocation so
   * waste = total allocated - requested is computed correctly.
   */
  const allocationTotals = {};

  if (Array.isArray(memoryData.blocks)) {
    memoryData.blocks.forEach((block) => {
      if (!block.free) {
        allocationTotals[block.allocationId] =
          (allocationTotals[block.allocationId] || 0) +
          Number(block.size || 0);
      }
    });
  }

  const memoryBlocks =
    Array.isArray(memoryData.blocks)
      ? memoryData.blocks.map((block) => {

          const allocationId =
            block.allocationId;

          const id =
            block.free
              ? "FREE"
              : `A${String(allocationId).padStart(3, "0")}`;

          const size =
            Number(block.size || 0);

          const requested =
            Number(block.requestedSize || 0);

          const waste =
            block.free || requested === 0
              ? 0
              : Math.max(
                  0,
                  (allocationTotals[allocationId] || size) -
                    requested
                );

          return {
            id,
            offset: Number(block.offset || 0),
            size,
            requested,
            status: block.free
              ? "FREE"
              : "USED",
            type:
              block.free
                ? "AVAILABLE"
                : allocationMode.toUpperCase(),
            waste,
          };

        })
      : [];


  /* =====================================================
     PERFORMANCE DATA

     Only real benchmark data exported by the C program
     (memoryData.performance) is shown. Nothing is invented.
     ===================================================== */

  const perf = memoryData.performance;

  const performanceData = perf
    ? [
        { strategy: "First Fit", ...perf.firstFit },
        { strategy: "Best Fit", ...perf.bestFit },
        { strategy: "Worst Fit", ...perf.worstFit },
      ].filter((row) => row.allocations !== undefined)
    : [];


  /* =====================================================
     LINUX SYSTEM MEMORY (values arrive in KB)
     ===================================================== */

  const formatKB = (kb) =>
    kb >= 1048576
      ? `${(kb / 1048576).toFixed(2)} GB`
      : `${(kb / 1024).toFixed(0)} MB`;


  /* =====================================================
     INTERNAL FRAGMENTATION PERCENTAGE
     ===================================================== */

  const fragmentationPercentage =
    totalMemory > 0
      ? (
          (internalFragmentation /
            totalMemory) *
          100
        )
      : 0;


  /* =====================================================
     CONNECTION STATUS
     ===================================================== */

  const connectionText =
    connected
      ? "LIVE FROM C ALLOCATOR"
      : "BACKEND DISCONNECTED";


  return (

    <div className="app">


      {/* =================================================
          HEADER
          ================================================= */}

      <header className="header">

        <div>

          <div className="eyebrow">
            LINUX MEMORY PROJECT
          </div>

          <h1>
            Dynamic Memory Allocation Dashboard
          </h1>

          <p className="subtitle">
            Live allocation, fragmentation, utilization and Linux system memory.
          </p>

        </div>


        <div className="connection-status">

          <span className="status-dot"></span>

          {connectionText}

        </div>

      </header>


      {/* =================================================
          KPI CARDS
          ================================================= */}

      <section className="kpi-grid">


        <div className="card kpi-card">

          <span className="kpi-label">
            Total Memory
          </span>

          <strong>
            {totalMemory} KB
          </strong>

          <small>
            Configured pool
          </small>

        </div>


        <div className="card kpi-card">

          <span className="kpi-label">
            Assigned Memory
          </span>

          <strong>
            {assignedMemory} KB
          </strong>

          <small>
            {activeAllocations} active allocations
          </small>

        </div>


        <div className="card kpi-card">

          <span className="kpi-label">
            Free Memory
          </span>

          <strong>
            {freeMemory} KB
          </strong>

          <small>
            {freeBlocks} available region
            {freeBlocks !== 1 ? "s" : ""}
          </small>

        </div>


        <div className="card kpi-card">

          <span className="kpi-label">
            Utilization
          </span>

          <strong>
            {utilization.toFixed(2)}%
          </strong>

          <small>
            Strategy: {strategy}
          </small>

        </div>


      </section>


      {/* =================================================
          MEMORY MAP
          ================================================= */}

      <section className="card section-card">


        <div className="section-heading">

          <div>

            <div className="eyebrow">
              LIVE MEMORY MAP
            </div>

            <h2>
              Physical memory layout
            </h2>

          </div>


          <div className="legend">

            <span>

              <i className="legend-dot assigned"></i>

              Assigned

            </span>


            <span>

              <i className="legend-dot free"></i>

              Free

            </span>


            <span>

              <i className="legend-dot waste"></i>

              Internal waste

            </span>

          </div>

        </div>


        <div className="memory-map">

          {memoryBlocks.map((block, index) => {

            const width =
              totalMemory > 0
                ? (block.size / totalMemory) * 100
                : 0;


            return (

              <div
                key={`${block.id}-${block.offset}-${index}`}
                className={`memory-block ${
                  block.status === "FREE"
                    ? "free-block"
                    : "used-block"
                }`}
                style={{
                  width: `${width}%`,
                }}
                title={`${block.id} | ${block.size} KB`}
              >

                <div className="block-id">

                  {block.status === "FREE"
                    ? "FREE"
                    : block.id}

                </div>


                <div className="block-size">

                  {block.size} KB

                </div>


                {block.waste > 0 && (

                  <div className="waste-label">

                    +{block.waste} KB waste

                  </div>

                )}

              </div>

            );

          })}

        </div>


        <div className="map-scale">

          <span>
            0 KB
          </span>

          <span>
            {Math.round(totalMemory / 3)} KB
          </span>

          <span>
            {Math.round((totalMemory * 2) / 3)} KB
          </span>

          <span>
            {totalMemory} KB
          </span>

        </div>


      </section>


      {/* =================================================
          ALLOCATION + FRAGMENTATION
          ================================================= */}

      <div className="two-column">


        {/* ===============================================
            ALLOCATION INFORMATION
            =============================================== */}

        <section className="card section-card">


          <div className="section-heading">

            <div>

              <div className="eyebrow">
                ALLOCATION INFORMATION
              </div>

              <h2>
                Active blocks
              </h2>

            </div>


            <div className="badge">

              {activeAllocations} active

            </div>

          </div>


          <div className="table-wrapper">

            <table>

              <thead>

                <tr>

                  <th>ID</th>

                  <th>RANGE</th>

                  <th>SIZE</th>

                  <th>REQUESTED</th>

                  <th>WASTE</th>

                  <th>TYPE</th>

                </tr>

              </thead>


              <tbody>

                {memoryBlocks
                  .filter(
                    (block) =>
                      block.status !== "FREE"
                  )
                  .map((block) => (

                    <tr key={`${block.id}-${block.offset}`}>

                      <td>

                        <span className="allocation-id">

                          {block.id}

                        </span>

                      </td>


                      <td>

                        {block.offset}-
                        {block.offset + block.size} KB

                      </td>


                      <td>

                        {block.size} KB

                      </td>


                      <td>

                        {block.requested > 0
                          ? `${block.requested} KB`
                          : "(segment)"}

                      </td>


                      <td>

                        <span
                          className={
                            block.waste > 0
                              ? "waste-value"
                              : "zero-value"
                          }
                        >

                          {block.waste} KB

                        </span>

                      </td>


                      <td>

                        <span className="type-pill">

                          {block.type}

                        </span>

                      </td>

                    </tr>

                  ))}


                {memoryBlocks.filter(
                  (block) =>
                    block.status !== "FREE"
                ).length === 0 && (

                  <tr>

                    <td
                      colSpan="6"
                      style={{
                        textAlign: "center",
                        padding: "30px",
                        opacity: 0.6,
                      }}
                    >

                      No active allocations

                    </td>

                  </tr>

                )}

              </tbody>

            </table>

          </div>


        </section>


        {/* ===============================================
            FRAGMENTATION
            =============================================== */}

        <section className="card section-card">


          <div className="section-heading">

            <div>

              <div className="eyebrow">
                FRAGMENTATION ANALYSIS
              </div>

              <h2>
                Memory health
              </h2>

            </div>

          </div>


          <div className="health-grid">


            <div className="health-card">

              <span>
                Internal
              </span>

              <strong>
                {internalFragmentation} KB
              </strong>

              <small>
                Waste inside allocations
              </small>

            </div>


            <div className="health-card">

              <span>
                External
              </span>

              <strong>
                {externalFragmentation} KB
              </strong>

              <small>
                Scattered free regions
              </small>

            </div>


            <div className="health-card">

              <span>
                Free Blocks
              </span>

              <strong>
                {freeBlocks}
              </strong>

              <small>
                Available regions
              </small>

            </div>


            <div className="health-card">

              <span>
                Largest Free
              </span>

              <strong>
                {largestFree} KB
              </strong>

              <small>
                Largest free region
              </small>

            </div>


          </div>


          <div className="fragmentation-bar">

            <div
              className="fragmentation-fill"
              style={{
                width: `${Math.min(
                  fragmentationPercentage,
                  100
                )}%`,
              }}
            ></div>

          </div>


          <div className="fragmentation-info">

            <span>
              Internal fragmentation
            </span>

            <strong>
              {fragmentationPercentage.toFixed(2)}%
            </strong>

          </div>


        </section>


      </div>


      {/* =================================================
          PERFORMANCE + TREND
          ================================================= */}

      <div className="two-column">


        {/* ===============================================
            PERFORMANCE
            =============================================== */}

        <section className="card chart-card">


          <div className="section-heading">

            <div>

              <div className="eyebrow">
                PERFORMANCE ANALYSIS
              </div>

              <h2>
                First Fit vs Best Fit vs Worst Fit
              </h2>

            </div>

          </div>


          <div className="chart-container">

            {performanceData.length === 0 ? (

              <div
                style={{
                  textAlign: "center",
                  padding: "120px 20px",
                  opacity: 0.6,
                }}
              >
                Performance data unavailable
              </div>

            ) : (

            <ResponsiveContainer
              width="100%"
              height={330}
            >

              <BarChart
                data={performanceData}
              >

                <CartesianGrid
                  strokeDasharray="3 3"
                  stroke="#243246"
                />

                <XAxis
                  dataKey="strategy"
                  stroke="#8192aa"
                />

                <YAxis
                  stroke="#8192aa"
                />

                <Tooltip
                  contentStyle={{
                    background: "#111a27",
                    border:
                      "1px solid #26364b",
                    borderRadius: "10px",
                    color: "#fff",
                  }}
                />

                <Legend />


                <Bar
                  dataKey="allocations"
                  name="Successful allocations"
                  fill="#38a7ff"
                  radius={[
                    5,
                    5,
                    0,
                    0,
                  ]}
                />


                <Bar
                  dataKey="failures"
                  name="Failures"
                  fill="#ff5964"
                  radius={[
                    5,
                    5,
                    0,
                    0,
                  ]}
                />

              </BarChart>

            </ResponsiveContainer>

            )}

          </div>

          {perf && performanceData.length > 0 && (
            <p className="proc-note">
              Same {perf.requests} allocation requests replayed on a{" "}
              {perf.poolSize} KB scratch pool ({perf.mode}).
            </p>
          )}


        </section>


        {/* ===============================================
            MEMORY TREND
            =============================================== */}

        <section className="card chart-card">


          <div className="section-heading">

            <div>

              <div className="eyebrow">
                MEMORY TREND
              </div>

              <h2>
                Pool utilization
              </h2>

            </div>

          </div>


          <div className="chart-container">

            <ResponsiveContainer
              width="100%"
              height={330}
            >

              <LineChart
                data={memoryTrend}
              >

                <CartesianGrid
                  strokeDasharray="3 3"
                  stroke="#243246"
                />


                <XAxis
                  dataKey="time"
                  stroke="#8192aa"
                />


                <YAxis
                  domain={[0, 100]}
                  stroke="#8192aa"
                  unit="%"
                />


                <Tooltip
                  contentStyle={{
                    background: "#111a27",
                    border:
                      "1px solid #26364b",
                    borderRadius: "10px",
                    color: "#fff",
                  }}
                />


                <Line
                  type="monotone"
                  dataKey="utilization"
                  name="Utilization"
                  stroke="#42e8c7"
                  strokeWidth={3}
                  dot={{ r: 4 }}
                />

              </LineChart>

            </ResponsiveContainer>

          </div>


        </section>


      </div>


      {/* =================================================
          LINUX PROC + SYSTEM SUMMARY
          ================================================= */}

      <div className="two-column">


        {/* ===============================================
            LINUX /PROC
            =============================================== */}

        <section className="card section-card">


          <div className="section-heading">


            <div>

              <div className="eyebrow">
                LINUX /PROC
              </div>

              <h2>
                System memory monitoring
              </h2>

            </div>


            <div className="live-badge">

              <span></span>

              LIVE SYSTEM

            </div>


          </div>


          <div className="proc-grid">


            <div className="proc-card">

              <span>
                Total
              </span>

              <strong>
                {systemMemory ? formatKB(systemMemory.total) : "—"}
              </strong>

            </div>


            <div className="proc-card">

              <span>
                Used
              </span>

              <strong>
                {systemMemory ? formatKB(systemMemory.used) : "—"}
              </strong>

            </div>


            <div className="proc-card">

              <span>
                Available
              </span>

              <strong>
                {systemMemory ? formatKB(systemMemory.available_kb) : "—"}
              </strong>

            </div>


            <div className="proc-card">

              <span>
                Utilization
              </span>

              <strong>
                {systemMemory ? `${systemMemory.utilization}%` : "—"}
              </strong>

            </div>


          </div>


          <p className="proc-note">

            {systemMemory
              ? "Read live from Linux "
              : "Unavailable. Could not read "}

            <code>
              /proc/meminfo
            </code>

            {" "}
            via the Node backend. Separate from the allocator pool.

          </p>


        </section>


        {/* ===============================================
            SYSTEM SUMMARY
            =============================================== */}

        <section className="card section-card">


          <div className="eyebrow">
            SYSTEM SUMMARY
          </div>

          <h2>
            Current state
          </h2>


          <div className="summary-list">


            <div>

              <span>
                Allocation mode
              </span>

              <strong>
                {allocationMode}
              </strong>

            </div>


            <div>

              <span>
                Strategy
              </span>

              <strong>
                {strategy}
              </strong>

            </div>


            <div>

              <span>
                Active allocations
              </span>

              <strong>
                {activeAllocations}
              </strong>

            </div>


            <div>

              <span>
                Free blocks
              </span>

              <strong>
                {freeBlocks}
              </strong>

            </div>


            <div>

              <span>
                Memory status
              </span>

              <strong className="normal-status">

                {utilization >= 90
                  ? "CRITICAL"
                  : utilization >= 70
                    ? "HIGH"
                    : "NORMAL"}

              </strong>

            </div>


          </div>


        </section>


      </div>


      {/* =================================================
          FOOTER
          ================================================= */}

      <footer>

        <span>
          Linux Dynamic Memory Allocation & Memory Monitoring System
        </span>


        <span>
          Dashboard • Live Mode
        </span>

      </footer>


    </div>

  );

}


export default App;
