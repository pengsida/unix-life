package strp.arrival;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.EditText;
import android.widget.Spinner;
import android.widget.TextView;
import android.widget.Toast;

import java.util.ArrayList;
import java.util.List;

public class LoginActivity extends Activity{
    public static final int VISITOR = 0;
    public static final int TOURIST = 1;
    public static final int GUIDE = 2;

    private final int REGISTER_RET = 0;
    private int spinner_identiy = TOURIST;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_login);

        EditText phone_text = (EditText) findViewById(R.id.login_phone_text);
        EditText pwd_text = (EditText) findViewById(R.id.login_pwd_text);

        // 设置登录按钮
        Button ok_btn = (Button) findViewById(R.id.login_OK_btn);
        ok_btn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Intent i = MainActivity.newIntent(LoginActivity.this, spinner_identiy);
                startActivity(i);
            }
        });

        // 设置取消按钮
        Button cancel_btn = (Button) findViewById(R.id.login_CANCEL_btn);
        cancel_btn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                setResult(RESULT_CANCELED);
                finish();
            }
        });

        // 设置注册按钮
        Button register_btn = (Button) findViewById(R.id.login_register_btn);
        register_btn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Intent intent = new Intent();
                intent.setClass(LoginActivity.this, RegisterActivity.class);
                startActivityForResult(intent, REGISTER_RET);
            }
        });

        // 选择游客还是导游的身份
        final Spinner spinner = (Spinner)findViewById(R.id.spinner_login);
//        普通视图
//        ArrayAdapter<CharSequence> spinner_adapter = ArrayAdapter.createFromResource(this, R.array.identity_array, android.R.layout.simple_dropdown_item_1line);
//        spinner.setAdapter(spinner_adapter);

//         自定义视图
        List<String> identityList = new ArrayList<String>();
        identityList.add("游客");
        identityList.add("导游");
        IdentityAdapter spinner_adapter = new IdentityAdapter(this, identityList);
        spinner.setAdapter(spinner_adapter);

        spinner.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener() {
            @Override
            public void onItemSelected(AdapterView<?> parent, View view, int position, long id) {
                Spinner spinnerParent = (Spinner) parent;
                View v = spinnerParent.getSelectedView();
                TextView textView = (TextView) v.findViewById(R.id.identity_text);
                String temp = textView.getText().toString();
                if (temp.equals("游客"))
                    spinner_identiy = TOURIST;
                else
                    spinner_identiy = GUIDE;
            }

            @Override
            public void onNothingSelected(AdapterView<?> parent) {

            }
        });

        Button visitorButton = (Button)findViewById(R.id.login_visitor_btn);
        visitorButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Intent i = MainActivity.newIntent(LoginActivity.this, VISITOR);
                startActivity(i);
            }
        });
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);
        switch (requestCode) {
            case REGISTER_RET:
                if (resultCode == RESULT_OK) {

                }
                break;
            default:
                break;
        }
    }
}
