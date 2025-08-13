import { Pool } from "pg";

export const pool = new Pool({
  user: "postgres",
  host: "localhost",
  database: "university",
  password: "muitosegura",
  port: 5432,
});
