const express = require("express");
const app = express();
const PORT = 5000;
const cors = require("cors");
const path = require("path");
const { execSync } = require("child_process");

app.use(express.json());
app.use(cors());

function runC(args) {
  const exe =
    process.platform === "win32"
      ? "c_code/output/student.exe"
      : "c_code/output/student";

  const cmd = `"${path.join(__dirname, exe)}" ${args.join(" ")}`;
  return execSync(cmd).toString();
}

// GET all
app.get("/api/students", (req, res) => {
  try {
    const out = runC(["list"]);
    res.json(JSON.parse(out || "[]"));
  } catch (err) {
    res.status(500).json({ error: err.message });
  }
});

// GET one
app.get("/api/students/:id", (req, res) => {
  try {
    const out = runC(["get", req.params.id]);
    const parsed = JSON.parse(out || "null");
    if (!parsed) return res.status(404).json({ error: "Not found" });
    res.json(parsed);
  } catch (err) {
    res.status(500).json({ error: err.message });
  }
});

// --- UPDATED ADD (POST) ---
app.post("/api/students", (req, res) => {
  try {
    // Retrieve new field: cgpa
    const { reg_num, name, age, course_name, cgpa } = req.body;
    if (!reg_num || !name || !cgpa)
      return res
        .status(400)
        .json({ error: "reg_num, name, and cgpa required" });

    // Command format: add reg_num name age course_name cgpa
    const out = runC([
      "add",
      reg_num,
      name,
      String(age),
      course_name,
      String(cgpa),
    ]);
    res.status(201).json(JSON.parse(out));
  } catch (err) {
    res.status(500).json({ error: err.message });
  }
});

// --- UPDATED UPDATE (PUT) ---
app.put("/api/students/:id", (req, res) => {
  try {
    // Retrieve new field: cgpa
    const { reg_num, name, age, course_name, cgpa } = req.body;

    // Command format: update id reg_num name age course_name cgpa
    const out = runC([
      "update",
      req.params.id,
      reg_num,
      name,
      String(age),
      course_name,
      String(cgpa),
    ]);
    res.json(JSON.parse(out));
  } catch (err) {
    res.status(500).json({ error: err.message });
  }
});

// DELETE (No change needed)
app.delete("/api/students/:id", (req, res) => {
  try {
    const out = runC(["delete", req.params.id]);
    res.json(JSON.parse(out));
  } catch (err) {
    res.status(500).json({ error: err.message });
  }
});

app.listen(PORT, () =>
  console.log(`Backend running at http://localhost:${PORT}`)
);
