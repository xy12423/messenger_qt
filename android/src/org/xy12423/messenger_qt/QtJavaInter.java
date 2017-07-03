package org.xy12423.messenger_qt;

import android.app.Notification;
import android.app.NotificationManager;
import android.content.Context;

public class QtJavaInter extends org.qtproject.qt5.android.bindings.QtActivity
{
    private static QtJavaInter instance;
    private static NotificationManager notificationManager;
    private static Notification.Builder builder;

    public QtJavaInter()
    {
        instance = this;
    }

    public static void notify(String title, String msg)
    {
        if (notificationManager == null) {
            notificationManager = (NotificationManager)instance.getSystemService(Context.NOTIFICATION_SERVICE);
            builder = new Notification.Builder(instance).setSmallIcon(R.drawable.notification_icon);
        }

        builder.setContentTitle(title);
        builder.setContentText(msg);
        notificationManager.notify(1, builder.build());
    }
    
    public static void notifyCancel()
    {
        if (notificationManager != null) {
            notificationManager.cancel(1);
        }
    }
}
