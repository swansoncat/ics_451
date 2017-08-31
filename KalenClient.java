import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.Socket;
import java.net.SocketAddress;
import java.net.UnknownHostException;


public class KalenClient {

	public static void main(String[] args) {
		int port = Integer.parseInt(args[0]);
		 try (
			 Socket s = new Socket("localhost", port);
			 PrintWriter out = new PrintWriter(s.getOutputStream(), true);
			 BufferedReader in = new BufferedReader(new InputStreamReader(s.getInputStream()));
		 ) {
			BufferedReader stdIN = new BufferedReader(new InputStreamReader(System.in));
			String fromServer = in.readLine();
			String fromUser = null;
			int count = 0;
			
			while (fromServer != null) {
				count++;
				System.out.println(fromServer);
				if (count >= 2) {
					fromUser= stdIN.readLine();
					out.println(fromUser);
					if (fromUser.equalsIgnoreCase("quit")) {
						System.exit(0);
					}
				}
				fromServer = in.readLine();
			}
		} catch (UnknownHostException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
			System.exit(1);
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
			System.exit(1);
		}

	}

}
