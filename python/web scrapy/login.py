import requests
import urllib
import urllib2

# 提交表单的方式

# <form action="http://post.oreilly.com/client/o/oreilly/forms/ quicksignup.cgi" id="example_form2" method="POST">
# <input name="client_token" type="hidden" value="oreilly" />
# <input name="subscribe" type="hidden" value="optin" />
# <input name="success_url" type="hidden" value="http://oreilly.com/store/
# newsletter-thankyou.html" />
# <input name="error_url" type="hidden" value="http://oreilly.com/store/
# newsletter-signup-error.html" />
# <input name="topic_or_dod" type="hidden" value="1" />
# <input name="source" type="hidden" value="orm-home-t1-dotd" /> <fieldset>
# <input class="email_address long" maxlength="200" name= "email_addr" size="25" type="text" value=
# "Enter your email here" />
# <button alt="Join" class="skinny" name="submit" onclick=
# "return addClickTracking('orm','ebook','rightrail','dod' );" value="submit">Join</button>
#         </fieldset>
# </form>

# post参数要求
# The name of the field (or fields) you want to submit with data
# The action attribute of the form itself
params = {}