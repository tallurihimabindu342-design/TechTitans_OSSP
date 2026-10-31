import express from "express";
import cors from "cors";
import fs from "fs";
import path from "path";

const app = express();
const PORT = 5000;

app.use(cors());
app.use(express.json());

/*
 * Must be the folder the C program is run from, because the
 * allocator writes memory_status.json to its working directory.
 */
const DATA_FILE =
    process.env.MEMORY_STATUS_FILE ||
    "/home/hima/linux-memory-project/memory_status.json";


/* =========================================================
   ALLOCATOR DATA  (written by the C program)
   ========================================================= */

app.get("/api/memory", (req, res) => {
    try {
        if (!fs.existsSync(DATA_FILE)) {
            return res.json({
                connected: false,
                message: "Waiting for C memory allocator data"
            });
        }

        const data = JSON.parse(fs.readFileSync(DATA_FILE, "utf8"));

        /*
         * The file survives after the C program exits, so check the
         * allocator process is still alive (Linux: /proc/<pid>).
         */
        if (data.pid && !fs.existsSync(`/proc/${data.pid}`)) {
            return res.json({
                connected: false,
                message: "C allocator is not running"
            });
        }

        res.json({ ...data, connected: true });

    } catch (error) {
        console.error(error);

        res.status(500).json({
            connected: false,
            error: "Unable to read memory data"
        });
    }
});


/* =========================================================
   LINUX SYSTEM MEMORY  (/proc/meminfo, values in KB)
   Separate from the allocator's simulated pool.
   ========================================================= */

app.get("/api/system-memory", (req, res) => {
    try {
        const raw = fs.readFileSync("/proc/meminfo", "utf8");

        const kb = (key) => {
            const m = raw.match(new RegExp(`^${key}:\\s+(\\d+)`, "m"));
            return m ? parseInt(m[1], 10) : 0;
        };

        const total = kb("MemTotal");
        const availableKb = kb("MemAvailable");
        const used = total - availableKb;

        res.json({
            available: true,
            total,
            available_kb: availableKb,
            used,
            utilization: total
                ? Number(((used / total) * 100).toFixed(1))
                : 0
        });

    } catch (error) {
        res.json({ available: false });
    }
});


app.get("/api/health", (req, res) => {
    res.json({
        status: "Backend running",
        service: "Linux Memory Dashboard"
    });
});

app.listen(PORT, () => {
    console.log("======================================");
    console.log(" Linux Memory Dashboard Backend");
    console.log("======================================");
    console.log(`Server running on http://localhost:${PORT}`);
    console.log(`Reading allocator data from ${DATA_FILE}`);
});
