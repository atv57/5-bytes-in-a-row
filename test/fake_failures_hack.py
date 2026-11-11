# Test only, not part of the patch. Run from the root of a kernel tree.

p = 'crypto/jitterentropy-kcapi.c'
s = open(p).read()

call = 'jent_read_entropy(rng->entropy_collector, rdata, dlen)'
assert call in s
s = s.replace(call, 'jent_debug_read(rng, rdata, dlen)')

helper = '''static int debug_fake_failures;
module_param(debug_fake_failures, int, 0644);

static int jent_debug_read(struct jitterentropy *rng, u8 *rdata,
			   unsigned int dlen)
{
	if (debug_fake_failures > 0) {
		debug_fake_failures--;
		return -3;
	}
	return jent_read_entropy(rng->entropy_collector, rdata, dlen);
}

static int jent_kcapi_random('''
s = s.replace('static int jent_kcapi_random(', helper, 1)

open(p, 'w').write(s)
