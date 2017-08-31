import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.Date;


public class KalenServer {

	public static void main(String[] args) {
		
		
		
		int port = Integer.parseInt(args[0]);
		Date date = new Date();
		
		try (
			ServerSocket server = new ServerSocket(port);
			Socket client = server.accept();
			//note: you will get an connection error if you use "new PrintWriter(client.getOutputStream());" and this program will not work. Not sure why.
			PrintWriter out = new PrintWriter(client.getOutputStream(), true);
			BufferedReader in = new BufferedReader(new InputStreamReader(client.getInputStream()));
		) {
			out.println("Welcome to Kalen Server");
			out.println(date.toString());
			String userInput = "start";
			while (!userInput.equalsIgnoreCase("quit")) {
				userInput = in.readLine();
				if (userInput.equalsIgnoreCase("date")) {
					out.println(date.toString());
				}
				else {		
					out.println("You typed in: " + userInput);
				}
			}
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}

	}

}
