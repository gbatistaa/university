import java.util.ArrayList;

public class ShowArrayThread extends Thread {
  private ArrayList<Integer> array;

  public ShowArrayThread(ArrayList<Integer> array) {
    this.array = array;
  }

  @Override
  public void run() {
    StringBuilder buffer = new StringBuilder("[");
    for (int i = 0; i < array.size(); i++) {
      buffer.append(array.get(i));
      if (i < array.size() - 1)
        buffer.append(", ");
    }
    buffer.append("]");
    System.out.println("Original: " + buffer);
  }
}
