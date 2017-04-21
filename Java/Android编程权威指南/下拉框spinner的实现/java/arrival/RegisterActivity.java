package strp.arrival;

import android.app.Activity;
import android.os.Bundle;
import android.view.View;
import android.widget.AdapterView;
import android.widget.Button;
import android.widget.Spinner;
import android.widget.TextView;

import java.util.ArrayList;
import java.util.List;

public class RegisterActivity extends Activity{

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_register);

        // 设置注册按钮
        Button ok_btn = (Button) findViewById(R.id.register_OK_btn);
        ok_btn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {

            }
        });

        // 设置取消按钮
        Button cancel_btn = (Button) findViewById(R.id.register_CANCEL_btn);
        cancel_btn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                setResult(RESULT_CANCELED);
                finish();
            }
        });

        final Spinner spinner = (Spinner)findViewById(R.id.spinner_register);
        List<String> identityList = new ArrayList<String>();
        identityList.add("游客");
        identityList.add("导游");
        IdentityAdapter identityAdapter = new IdentityAdapter(RegisterActivity.this, identityList);
        spinner.setAdapter(identityAdapter);
        spinner.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener() {
            @Override
            public void onItemSelected(AdapterView<?> parent, View view, int position, long id) {
                Spinner spinnerParent = (Spinner)parent;
                View v = (View)spinnerParent.getSelectedView();
                TextView textView = (TextView)v.findViewById(R.id.identity_text);
                String temp = textView.getText().toString();
            }

            @Override
            public void onNothingSelected(AdapterView<?> parent) {

            }
        });
    }
}
