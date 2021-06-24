public class MultiThread implements Runnable
{
    @Override
    public void run() {
        int sum = 0;
        for (int index = 0; index < 10; index ++)
        {
            sum += index;
            System.out.println(Thread.currentThread() + Integer.toString(sum));
        }
        System.out.println(Thread.currentThread() + "최종 합: " + sum);
    }
    public static void main(String[] args) {
        Runnable multithread = new MultiThread();
        Thread subThread1 = new Thread(multithread);
        Thread subThread2 = new Thread(multithread);
        subThread1.start();
        subThread2.start();
        
    }
}