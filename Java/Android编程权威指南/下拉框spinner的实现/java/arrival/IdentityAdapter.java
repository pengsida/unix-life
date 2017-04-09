package strp.arrival;

import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.TextView;

import java.util.List;

/**
 * Created by pengsida on 2017/3/29.
 */

public class IdentityAdapter extends BaseAdapter
{
    private List<String> mList;
    private Context mContext;

    public IdentityAdapter(Context context, List<String> stringList)
    {
        mList = stringList;
        mContext = context;
    }

    @Override
    public int getCount()
    {
        return mList.size();
    }

    @Override
    public Object getItem(int position)
    {
        return mList.get(position);
    }

    @Override
    public long getItemId(int position)
    {
        return position;
    }

    @Override
    public View getView(int position, View view, ViewGroup parent)
    {
        LayoutInflater layoutInflater = LayoutInflater.from(mContext);
        view = layoutInflater.inflate(R.layout.identity_item, null);
        if (view != null)
        {
            TextView text = (TextView)view.findViewById(R.id.identity_text);
            text.setText(mList.get(position));
        }
        return view;
    }
}
