import { useState, useEffect } from "react";
import axios from "axios";
import "./App.css";

function App() {
  const [students, setStudents] = useState([]);
  const [form, setForm] = useState({
    reg_num: "",
    name: "",
    age: "",
    course_name: "",
    cgpa: "",
  });
  const [editingId, setEditingId] = useState(null);

  const API = "http://localhost:5000/api/students";

  useEffect(() => {
    loadStudents();
  }, []);

  const loadStudents = async () => {
    try {
      const res = await axios.get(API);
      setStudents(res.data);
    } catch (err) {
      console.error(err);
    }
  };

  const handleChange = (e) => {
    setForm({ ...form, [e.target.name]: e.target.value });
  };

  const submitForm = async () => {
    try {
      if (
        !form.reg_num ||
        !form.name ||
        !form.age ||
        !form.course_name ||
        !form.cgpa
      ) {
        alert("All fields are required!");
        return;
      }

      const formDataToSend = { ...form, cgpa: parseFloat(form.cgpa) };

      if (editingId) {
        await axios.put(`${API}/${editingId}`, formDataToSend);
        setEditingId(null);
      } else {
        await axios.post(API, formDataToSend);
      }

      setForm({ reg_num: "", name: "", age: "", course_name: "", cgpa: "" });
      loadStudents();
    } catch (err) {
      console.error(err);
      alert("Error submitting form!");
    }
  };

  // Argument is now named 'id' for clarity
  const deleteStudent = async (id) => {
    try {
      // Use 'id' in the API path
      await axios.delete(`${API}/${id}`);
      loadStudents();
    } catch (err) {
      console.error(err);
    }
  };

  const editStudent = (student) => {
    setForm({
      reg_num: student.reg_num,
      name: student.name,
      age: student.age,
      course_name: student.course_name,
      cgpa: student.cgpa,
    });
    // FIX: Use student.id instead of student._id
    setEditingId(student.id);
  };

  return (
    <div className="container">
      <h1>Student Record System 🧑‍🎓</h1>
      <div className="form-box">
        <input
          type="text"
          name="reg_num"
          placeholder="Registration Number"
          value={form.reg_num}
          onChange={handleChange}
        />
        <input
          type="text"
          name="name"
          placeholder="Student Name"
          value={form.name}
          onChange={handleChange}
        />
        <input
          type="number"
          name="age"
          placeholder="Age"
          value={form.age}
          onChange={handleChange}
        />
        <input
          type="text"
          name="course_name"
          placeholder="Course Name"
          value={form.course_name}
          onChange={handleChange}
        />
        <input
          type="text"
          name="cgpa"
          placeholder="CGPA (e.g., 9.2)"
          value={form.cgpa}
          onChange={handleChange}
        />
        <button onClick={submitForm}>
          {editingId ? "Update Student" : "Add Student"}
        </button>
      </div>
      <h2>Student List</h2>
      <table>
        <thead>
          <tr>
            <th>Reg. Number</th>
            <th>Name</th>
            <th>Age</th>
            <th>Course Name</th>
            <th>CGPA</th>
            <th>Actions</th>
          </tr>
        </thead>
        <tbody>
          {students.length === 0 ? (
            <tr>
              <td colSpan="6">No students found.</td>
            </tr>
          ) : (
            students.map((s) => (
              <tr key={s.id}>
                <td>{s.reg_num}</td>
                <td>{s.name}</td>
                <td>{s.age}</td>
                <td>{s.course_name}</td>
                <td>{s.cgpa}</td>
                <td>
                  <button onClick={() => editStudent(s)}>Edit</button>
                  <button onClick={() => deleteStudent(s.id)}>Delete</button>
                </td>
              </tr>
            ))
          )}
        </tbody>
      </table>
    </div>
  );
}

export default App;
