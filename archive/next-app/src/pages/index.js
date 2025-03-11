import Link from 'next/link';

export default function Home() {
  return (
    <div>
      <h1>Home Page</h1>
      <nav>
        <Link href="/dashboard">About</Link>
        <Link href="/dashboard">Contact</Link>
      </nav>
    </div>
  );
}
